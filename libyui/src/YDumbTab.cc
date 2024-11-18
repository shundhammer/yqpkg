/*
  Copyright (C) 2000-2012 Novell, Inc
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

  File:		YDumbTab.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YDumbTab.h"
#include "YShortcut.h"

using std::string;


struct YDumbTabPrivate
{
    YDumbTabPrivate()
	{}

    bool dummy;
};




YDumbTab::YDumbTab( YWidget * parent )
    : YSelectionWidget( parent,
			"",	// label
			true )	// enforceSingleSelection
    , priv( new YDumbTabPrivate )
{
    YUI_CHECK_NEW( priv );
    setChildrenManager( new YSingleWidgetChildManager( this ) );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YDumbTab::~YDumbTab()
{
    // NOP
}


void
YDumbTab::addItem( YItem * item )
{
    YSelectionWidget::addItem( item );
}


bool
YDumbTab::stretchable( YUIDimension dim ) const
{
    if ( hasChildren() )
	return firstChild()->stretchable( dim );
    else
	return YWidget::stretchable( dim );
}


string
YDumbTab::debugLabel() const
{
    string str = widgetClass();

    for ( YItemConstIterator it = itemsBegin();
	  it != itemsEnd();
	  ++it )
    {
	str += " [" + (*it)->label() + "]";
    }

    return str;
}



const YPropertySet &
YDumbTab::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemID  	Value		The currently selected item (tab page)
	 * @property itemID  	CurrentItem	The currently selected item (tab page)
	 * @property itemList	Items		All items (all tab pages)
	 */
	propSet.add( YProperty( YUIProperty_Value,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YDumbTab::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentItem 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YDumbTab::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentItem 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
