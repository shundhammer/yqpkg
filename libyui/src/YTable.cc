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

  File:		YTable.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YTable.h"

using std::string;


struct YTablePrivate
{
    YTablePrivate( YTableHeader * header )
	: header( header )
	, keepSorting( false )
	, immediateMode( false )
    {
    }

    YTableHeader *	header;
    bool		keepSorting;
    bool		immediateMode;
};




YTable::YTable( YWidget * parent, YTableHeader * header, bool multiSelection )
    : YSelectionWidget( parent,
			"",	// label
			! multiSelection ) // enforceSingleSelection
    , priv( new YTablePrivate( header ) )
{
    YUI_CHECK_PTR( header );
    YUI_CHECK_NEW( priv   );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YTable::~YTable()
{
    if ( priv->header )
	delete priv->header;
}


void
YTable::setTableHeader( YTableHeader * newHeader )
{
    YUI_CHECK_PTR( newHeader );

    if ( priv->header->columns() != newHeader->columns() )
	deleteAllItems();

    delete priv->header;
    priv->header = newHeader;
}


int
YTable::columns() const
{
    return priv->header->columns();
}


bool
YTable::hasColumn( int column ) const
{
    return priv->header->hasColumn( column );
}


string
YTable::header( int column ) const
{
    return priv->header->header( column );
}


YAlignmentType
YTable::alignment( int column ) const
{
    return priv->header->alignment( column );
}


bool
YTable::immediateMode() const
{
    return priv->immediateMode;
}


void
YTable::setImmediateMode( bool immediateMode )
{
    priv->immediateMode = immediateMode;

    if ( immediateMode )
	setNotify( true );
}


bool
YTable::keepSorting() const
{
    return priv->keepSorting;
}


void
YTable::setKeepSorting( bool keepSorting )
{
    priv->keepSorting = keepSorting;
}


bool
YTable::hasMultiSelection() const
{
    return ! YSelectionWidget::enforceSingleSelection();
}

YItem *
YTable::findItem( const string & wantedItemLabel, int column ) const
{
    return YTable::findItem( wantedItemLabel, column, itemsBegin(), itemsEnd() );
}


YItem *
YTable::findItem( const string & wantedItemLabel,
                  int                 column,
                  YItemConstIterator  begin,
                  YItemConstIterator  end ) const
{
    if ( ! hasColumn( column ) )
        return nullptr;

    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        auto * item = dynamic_cast<YTableItem *>(*it);

        if ( item && item->label( column ) == wantedItemLabel )
            return item;
    }

    return nullptr;
}

const YPropertySet &
YTable::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemID	Value		The currently selected item
	 * @property itemID	CurrentItem	The currently selected item
	 * @property itemList	Items		All items
	 * @property itemList	SelectedItems	All currently selected items
	 * @property string	Cell		One cell (one column of one item)
	 * @property integer	Cell		(ChangeWidget only) One cell as integer
	 * @property Icon(...)	Cell		Icon for one cell
	 * @property string	Item		Alias for Cell
	 * @property string	Item		QueryWidget only: Return one complete item
	 * @property string	IconPath	Base path for icons
	 * @property bool	MultiSelection	Flag: User can select multiple items (read-only)
         *
	 * @property map<ItemID, string> OpenItems 	Map of IDs of all open items (read-only)
         *                                              to either "ID" or "Text":
         *                                              { "myFirstItemID": "ID",
         *                                                "myThirdItem":   "Text" }
	 */
	propSet.add( YProperty( YUIProperty_Value,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_SelectedItems,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Cell,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Item,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_MultiSelection,	YBoolProperty,   true ) ); // read-only
	propSet.add( YProperty( YUIProperty_OpenItems,		YOtherProperty,  true ) ); // read-only
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YTable::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentItem 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_SelectedItems	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Cell		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Item 		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_IconPath 	)	setIconBasePath( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YTable::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentItem 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_SelectedItems	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Cell		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Item 		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_IconPath	)	return YPropertyValue( iconBasePath() );
    else if ( propertyName == YUIProperty_MultiSelection )      return YPropertyValue( hasMultiSelection() );
    else if ( propertyName == YUIProperty_OpenItems 	)	return YPropertyValue( YOtherProperty );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
