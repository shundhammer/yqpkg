/*
  Copyright (c) [2019] SUSE LLC
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		YItemSelector.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YItemSelector.h"

#define DEFAULT_VISIBLE_ITEMS	3

using std::string;


struct YItemSelectorPrivate
{
    YItemSelectorPrivate()
	: visibleItems( DEFAULT_VISIBLE_ITEMS )
	{}

    int				visibleItems;
    YItemCustomStatusVector	customStates;
};


YItemSelector::YItemSelector( YWidget *	   parent,
			      bool	   enforceSingleSelection )
    : YSelectionWidget( parent,
			"",	// label
			enforceSingleSelection )
    , priv( new YItemSelectorPrivate )
{
    YUI_CHECK_NEW( priv );

    init();
}


YItemSelector::YItemSelector( YWidget *				   parent,
			      const YItemCustomStatusVector &	   customStates )
    : YSelectionWidget( parent,
			"",	// label
			false ) // enforceSingleSelection )
    , priv( new YItemSelectorPrivate )
{
    YUI_CHECK_NEW( priv );

    priv->customStates = customStates;
    init();
    checkCustomStates();
}


YItemSelector::~YItemSelector()
{
    // NOP
}


void YItemSelector::init()
{
    setEnforceInitialSelection( false );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


const char *
YItemSelector::widgetClass() const
{
    if ( usingCustomStatus() )
        return "YCustomStatusItemSelector";

    return enforceSingleSelection() ? "YSingleItemSelector" : "YMultiItemSelector";
}


int
YItemSelector::visibleItems() const
{
    return priv->visibleItems;
}


void YItemSelector::setVisibleItems( int newVal )
{
    if ( newVal < 1 )
	newVal = 1;

    priv->visibleItems = newVal;
}


void YItemSelector::setItemStatus( YItem * item, int status )
{
    if ( usingCustomStatus() )
    {
        YUI_CHECK_INDEX( status, -1, customStatusCount() - 1 );
        item->setStatus( status );
        updateCustomStatusIndicator( item );

        // Intentionally NOT calling the parent class implementation since that
        // would only store 0 or 1 as the item's status.
    }
    else
    {
        // Let the parent class handle this and also take care of enforcing
        // single selection (if configured).

        YSelectionWidget::setItemStatus( item, status );
    }
}


bool YItemSelector::usingCustomStatus() const
{
    return ! priv->customStates.empty();
}


int YItemSelector::customStatusCount() const
{
    return priv->customStates.size();
}


const YItemCustomStatus &
YItemSelector::customStatus( int index )
{
    // std::vector::at() does bounds checking of 'index', possibly throwing a
    // std::out_of_range exception.
    //
    // std::vector::operator[] does not do any checking and may just cause a
    // segfault or misbehave in spectacular ways.

    return priv->customStates.at( (YItemCustomStatusVector::size_type) index );
}


bool YItemSelector::validCustomStatusIndex( int index ) const
{
    return index >= 0 && index < customStatusCount();
}


void YItemSelector::checkCustomStates()
{
    if ( priv->customStates.size() < 2 )
        YUI_THROW( YUIException( "Need at least 2 different custom status value definitions" ) );

    int maxStatus = priv->customStates.size() - 1;

    for ( int i=0; i <= maxStatus; ++i )
    {
	YItemCustomStatus & status = priv->customStates.at( (YItemCustomStatusVector::size_type) i );

	if ( status.nextStatus() > maxStatus )
	{
	    yuiError() << "Invalid nextStatus " << status.nextStatus()
		       << " for status #" << i
		       << endl;

	    status.setNextStatus( -1 );
	}
	else if ( status.nextStatus() < -1 )
	    status.setNextStatus( -1 );
        else
        {
            yuiDebug() << "Status #" << i << ": next status: #" << status.nextStatus() << endl;
        }
    }
}


int YItemSelector::cycleCustomStatus( int oldStatus )
{
    if ( ! validCustomStatusIndex( oldStatus ) )
    {
        yuiDebug() << "Invalid old status: " << oldStatus << endl;
        return oldStatus;
    }
    else
        return priv->customStates.at( oldStatus ).nextStatus();
}


const YPropertySet &
YItemSelector::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemID	Value		The (first) currently selected item
	 * @property itemID	CurrentItem	The (first) currently selected item
	 * @property itemList	SelectedItems	All currently selected items
	 * @property itemList	Items		All items
	 * @property statusMap  ItemStatus	The (numeric) status values of the items
	 * @property integer	VisibleItems	Number of items that are visible without scrolling
	 * @property string	IconPath	Base path for icons
	 */
	propSet.add( YProperty( YUIProperty_Value,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_SelectedItems,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_ItemStatus,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_VisibleItems,	YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YItemSelector::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentItem	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_SelectedItems	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_ItemStatus    )	return false; // Needs special handling
    else if ( propertyName == YUIProperty_VisibleItems	)	setVisibleItems( val.integerVal() );
    else if ( propertyName == YUIProperty_IconPath	)	setIconBasePath( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YItemSelector::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentItem	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_SelectedItems	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_ItemStatus    )	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_VisibleItems	)	return YPropertyValue( visibleItems() );
    else if ( propertyName == YUIProperty_IconPath	)	return YPropertyValue( iconBasePath() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
