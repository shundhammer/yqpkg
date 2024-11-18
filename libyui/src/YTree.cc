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

  File:		YTree.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YSelectionWidget.h"
#include "YTree.h"
#include "YTreeItem.h"

using std::string;
using std::vector;


struct YTreePrivate
{
    YTreePrivate()
	: immediateMode( false )
	{}

    bool immediateMode;
};


YTree::YTree( YWidget *      parent,
              const string & label,
              bool           multiSelection,
              bool           recursiveSelection )
    : YSelectionWidget( parent, label,
			! multiSelection,
			recursiveSelection )
    , priv( new YTreePrivate() )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YTree::~YTree()
{
    // NOP
}


bool
YTree::immediateMode() const
{
    return priv->immediateMode;
}


void
YTree::setImmediateMode( bool immediateMode )
{
    priv->immediateMode = immediateMode;

    if ( immediateMode )
	setNotify( true );
}


void
YTree::addItems( const YItemCollection & itemCollection )
{
    YSelectionWidget::addItems( itemCollection );
    rebuildTree();
}


const YPropertySet &
YTree::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemID  		Value		The currently selected item
	 * @property itemID  		CurrentItem	The currently selected item
	 * @property list<itemID>  	CurrentBranch	List of IDs of current branch from root to current item
	 * @property itemList		Items		All items
	 * @property string             Label		Caption above the tree
	 * @property string             IconPath	Base path for icons
         * @property bool               MultiSelection  Flag: User can select multiple items (read-only)
         *
	 * @property map<ItemID, string> OpenItems 	Map of IDs of all open items (read-only)
         *                                              to either "ID" or "Text":
         *                                              { "myFirstItemID": "ID",
         *                                                "myThirdItem":   "Text" }
	 */
	propSet.add( YProperty( YUIProperty_Value,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentBranch,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
        propSet.add( YProperty( YUIProperty_SelectedItems,      YOtherProperty   ) );
        propSet.add( YProperty( YUIProperty_MultiSelection,     YBoolProperty,   true ) ); // read-only
	propSet.add( YProperty( YUIProperty_OpenItems,		YOtherProperty,	 true ) ); // read-only
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YTree::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentItem 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentBranch )	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_SelectedItems )       return false; // Needs special handling
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else if ( propertyName == YUIProperty_IconPath 	)	setIconBasePath( val.stringVal() );

    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YTree::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentItem 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentBranch )	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else if ( propertyName == YUIProperty_IconPath	)	return YPropertyValue( iconBasePath() );
    else if ( propertyName == YUIProperty_SelectedItems )       return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_MultiSelection )      return YPropertyValue( hasMultiSelection() );
    else if ( propertyName == YUIProperty_OpenItems 	)	return YPropertyValue( YOtherProperty );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}

bool
YTree::hasMultiSelection() const
{
    return ! YSelectionWidget::enforceSingleSelection();
}


YTreeItem *
YTree::findItem( const vector<string> & path ) const
{
    return findItem( path.begin(), path.end(),
                     itemsBegin(), itemsEnd() );
}


YTreeItem *
YTree::findItem( vector<string>::const_iterator path_begin,
                 vector<string>::const_iterator path_end,
                 YItemConstIterator             begin,
                 YItemConstIterator             end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        YTreeItem * item = dynamic_cast<YTreeItem *>( *it );

        if ( ! item )
            return 0;

        if ( item->label() == *path_begin )
        {
            if ( std::next( path_begin ) == path_end )
            {
                return item;
            }

            // Recursively search child items
            YTreeItem * result = findItem( ++path_begin, path_end,
                                           item->childrenBegin(), item->childrenEnd() );

            if ( result )
                return result;
        }
    }

    return 0;
}
