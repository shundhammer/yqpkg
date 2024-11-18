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

  File:		YSelectionWidget.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include <algorithm>
#include "YSelectionWidget.h"
#include "YUIException.h"
#include "YApplication.h"

using std::string;


struct YSelectionWidgetPrivate
{
    YSelectionWidgetPrivate( const string & label,
			     bool	    enforceSingleSelection,
			     bool	    recursiveSelection )
	: label( label )
	, enforceSingleSelection( enforceSingleSelection )
        , enforceInitialSelection( true )
	, recursiveSelection ( recursiveSelection )
	{}

    string		label;
    bool		enforceSingleSelection;
    bool                enforceInitialSelection;
    bool		recursiveSelection;
    string		iconBasePath;
    YItemCollection	itemCollection;
};




YSelectionWidget::YSelectionWidget( YWidget *		parent,
				    const string &	label,
				    bool		enforceSingleSelection ,
				    bool		recursiveSelection )
    : YWidget( parent )
    , priv( new YSelectionWidgetPrivate( label, enforceSingleSelection, recursiveSelection ) )
{
    YUI_CHECK_NEW( priv );

    if ( enforceSingleSelection && recursiveSelection )
	YUI_THROW( YUIException( "recursiveSelection is only available for multiSelection Widgets."));

}


YSelectionWidget::~YSelectionWidget()
{
    deleteAllItems();
}


void YSelectionWidget::deleteAllItems()
{
    YItemIterator it = itemsBegin();

    while ( it != itemsEnd() )
    {
	YItem * item = *it;
	++it;
	delete item;

	// No need to check for item->hasChildren() and iterate recursively
	// over the children: The item will take care of its children in its
	// destructor.
    }

    priv->itemCollection.clear();
}


string YSelectionWidget::label() const
{
    return priv->label;
}


void YSelectionWidget::setLabel( const string & newLabel )
{
    priv->label = newLabel;
}


bool YSelectionWidget::enforceSingleSelection() const
{
    return priv->enforceSingleSelection;
}


void YSelectionWidget::setEnforceInitialSelection( bool newVal )
{
    if ( priv->enforceSingleSelection )
        priv->enforceInitialSelection = newVal;
}


bool YSelectionWidget::enforceInitialSelection() const
{
    return priv->enforceInitialSelection && priv->enforceSingleSelection;
}


bool YSelectionWidget::recursiveSelection() const
{
    return priv->recursiveSelection;
}



void YSelectionWidget::setEnforceSingleSelection( bool enforceSingleSelection )
{
    priv->enforceSingleSelection = enforceSingleSelection;
}


void YSelectionWidget::setShortcutString( const std::string & str )
{
    setLabel( str );
    shortcutChanged();
}


void YSelectionWidget::setIconBasePath( const string & basePath )
{
    priv->iconBasePath = basePath;
}


string YSelectionWidget::iconBasePath() const
{
    return priv->iconBasePath;
}


string YSelectionWidget::iconFullPath( const string & iconName ) const
{
    string fullPath;

    if ( ! iconName.empty() )
    {
	if ( iconName[0] == '/' )
	    return iconName;

	if ( priv->iconBasePath.empty() ||
	     priv->iconBasePath[0] != '/' )
	{
	    return YUI::yApp()->iconLoader()->findIcon( iconName );
	}

	fullPath += priv->iconBasePath + "/" + iconName;
    }

    return fullPath;
}


string YSelectionWidget::iconFullPath( YItem * item ) const
{
    if ( item )
	return iconFullPath( item->iconName() );
    else
	return "";
}


void YSelectionWidget::addItem( YItem * item )
{
    YUI_CHECK_PTR( item );

    if ( item->parent() )
    {
	YUI_THROW( YUIException( "Item already owned by parent item -"
				 " call addItem() only for toplevel items!" ) );
    }

    // Add the new item to the item list

    priv->itemCollection.push_back( item );
    item->setIndex( priv->itemCollection.size() - 1 );

    // yuiDebug() << "Adding item \"" << item->label() << "\"" << endl;

    //
    // Enforce single selection (if applicable)
    //

    if ( priv->enforceSingleSelection )
    {
	YItem * newItemSelected = 0;

	if ( item->selected() )
	{
	   newItemSelected = item;
	}
	else
	{
	   newItemSelected = findSelectedItem( item->childrenBegin(),
					       item->childrenEnd() );
	}

	if ( newItemSelected )
	{
	    // This looks expensive, but it is not: Even though deselectAllItems()
	    // searches the complete item list and deselects them all.
	    //
	    // This prevents that the calling application does this systematically wrong
	    // and sets the "selected" flag for more items or children.

	    deselectAllItems();
	    newItemSelected->setSelected( true );
	}


        if ( priv->enforceInitialSelection )
        {
            // Make sure there is one item selected initially.
            //
            // If any other subsequently added items are to be selected, they will
            // override this initial selection.

            if ( priv->itemCollection.size() == 1 )
                item->setSelected( true );
        }
    }
}


void YSelectionWidget::addItem( const string &	itemLabel,
				const string &	iconName,
				bool		selected )
{
    YItem * item = new YItem( itemLabel, iconName, selected );
    YUI_CHECK_NEW( item );
    addItem( item );
}


void YSelectionWidget::addItem( const string & itemLabel, bool selected )
{
    addItem( itemLabel, "", selected );
}


void YSelectionWidget::addItems( const YItemCollection & itemCollection )
{
    OptimizeChanges below( *this ); // Delay screen updates until this block is left
    priv->itemCollection.reserve( priv->itemCollection.size() + itemCollection.size() );

    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it );

	// No need to check for (*it)->hasChildren() and iterate recursively
	// over the children: Any children of this item simply remain in this
	// item's YItemCollection.
    }
}


YItemIterator
YSelectionWidget::itemsBegin()
{
    return priv->itemCollection.begin();
}

YItemConstIterator
YSelectionWidget::itemsBegin() const
{
    return priv->itemCollection.begin();
}


YItemIterator
YSelectionWidget::itemsEnd()
{
    return priv->itemCollection.end();
}


YItemConstIterator
YSelectionWidget::itemsEnd() const
{
    return priv->itemCollection.end();
}


bool YSelectionWidget::hasItems() const
{
    return ! priv->itemCollection.empty();
}


int YSelectionWidget::itemsCount() const
{
    return priv->itemCollection.size();
}


YItem *
YSelectionWidget::firstItem() const
{
    if ( priv->itemCollection.empty() )
	return 0;
    else
	return priv->itemCollection.front();
}


YItem *
YSelectionWidget::itemAt( int index ) const
{
    if ( index < 0 || index >= (int) priv->itemCollection.size() )
	return 0;

    return priv->itemCollection[ index ];
}


YItem *
YSelectionWidget::selectedItem()
{
    return findSelectedItem( itemsBegin(), itemsEnd() );
}


YItem *
YSelectionWidget::findSelectedItem( YItemConstIterator begin,
				    YItemConstIterator end )
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	const YItem * item = *it;

	if ( item->selected() )
	{
	    return *it;
	}
	if ( item->hasChildren() )
	{
	    YItem * selectedItem = findSelectedItem( item->childrenBegin(),
						     item->childrenEnd() );
	    if ( selectedItem )
	    {
		// yuiDebug() << "Selected item: \"" << selectedItem->label() << "\"" << endl;
		return selectedItem;
	    }
	}
    }

    return 0;
}


YItemCollection
YSelectionWidget::selectedItems()
{
    YItemCollection selectedItems;
    findSelectedItems( selectedItems, itemsBegin(), itemsEnd() );

    return selectedItems;
}


void
YSelectionWidget::findSelectedItems( YItemCollection &	selectedItems,
				     YItemConstIterator begin,
				     YItemConstIterator end )
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	YItem * item = *it;

	if ( item->selected() )
	    selectedItems.push_back( item );

	if ( item->hasChildren() )
	{
	    findSelectedItems( selectedItems,
			       item->childrenBegin(),
			       item->childrenEnd() );
	}
    }
}


bool YSelectionWidget::hasSelectedItem()
{
    return selectedItem() != 0;
}


void YSelectionWidget::selectItem( YItem * item, bool selected )
{
    YUI_CHECK_PTR( item );

    if ( ! itemsContain( item ) )
	YUI_THROW( YUIException( "Item does not belong to this widget" ) );

    if ( priv->enforceSingleSelection && selected )
    {
	YItem * oldSelectedItem = selectedItem();

	if ( oldSelectedItem )
	    oldSelectedItem->setSelected( false );
    }


    if ( recursiveSelection() && item->hasChildren() )
    {
	for ( YItemIterator it = item->childrenBegin(); it != item->childrenEnd(); ++it )
	{
	    YItem * item = *it;
	    selectItem(item, selected );
	    item->setSelected( selected );
	}
    }

    item->setSelected( selected );
}


void YSelectionWidget::setItemStatus( YItem * item, int status )
{
    selectItem( item, status != 0 );
}


bool YSelectionWidget::itemsContain( YItem * wantedItem ) const
{
    return itemsContain( wantedItem, itemsBegin(), itemsEnd() );
}



bool
YSelectionWidget::itemsContain( YItem *			wantedItem,
				YItemConstIterator	begin,
				YItemConstIterator	end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	const YItem * item = *it;

	if ( item == wantedItem )
	    return true;

	if ( item->hasChildren() )
	{
	    if ( itemsContain( wantedItem,
			       item->childrenBegin(),
			       item->childrenEnd() ) )
	    {
		return true;
	    }
	}
    }

    return false;
}


void YSelectionWidget::deselectAllItems()
{
    deselectAllItems( itemsBegin(), itemsEnd() );
}


void YSelectionWidget::deselectAllItems( YItemIterator	begin,
					 YItemIterator	end )
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	YItem * item = *it;

	item->setSelected( false );

	if ( item->hasChildren() )
	    deselectAllItems( item->childrenBegin(), item->childrenEnd() );
    }
}


YItem *
YSelectionWidget::findItem( const string & wantedItemLabel ) const
{
    return findItem( wantedItemLabel, itemsBegin(), itemsEnd() );
}


YItem *
YSelectionWidget::findItem( const string &	wantedItemLabel,
			    YItemConstIterator	begin,
			    YItemConstIterator	end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	YItem * item = *it;

	if ( item->label() == wantedItemLabel )
	    return item;

	if ( item->hasChildren() )
	{
	    YItem * wantedItem = findItem( wantedItemLabel,
					   item->childrenBegin(),
					   item->childrenEnd() );
	    if ( wantedItem )
		return wantedItem;
	}
    }

    return 0;
}


void YSelectionWidget::dumpItems() const
{
    yuiMilestone() << "Items:" << endl;

    for ( YItemConstIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
        string status;

        switch ( (*it)->status() )
        {
            case 0: status = "[ ]";     break;
            case 1: status = "[x]";     break;
            default:
                {
                    char buffer[80];
                    sprintf( buffer, "[%d]", (*it)->status() );
                    status = buffer;
                }
        }

        yuiMilestone() << "  " << status << " "
                       << (*it)->label()
                       << endl;
    }

    yuiMilestone() << "---" << endl;
}
