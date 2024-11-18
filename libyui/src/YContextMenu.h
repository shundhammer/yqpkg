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

  File:		YContextMenu.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YContextMenu_h
#define YContextMenu_h

#include "YSelectionWidget.h"
#include "YMenuItem.h"

class YMenuItem;
class YContextMenuPrivate;


/**
 * ContextMenu: Similar to PushButton, but with several actions: Upon clicking
 * on a ContextMenu (or activating it with the keyboard), a pop-up menu opens
 * where the user can activate an action. Menu items in that pop-up menu can
 * have submenus (that will pop up in separate pop-up menus).
 *
 * Internally, this widget is more similar to the Tree widget. The difference
 * is that it does not keep a "selected" status, but triggers an action right
 * away, just like a PushButton. Like PushButton, ContextMenu sends an event
 * right away when the user selects an item (clicks on a menu item or activates
 * it with the keyboard). Items that have a submenu never send an event, they
 * simply open their submenu when activated.
 *
 * Note: unlike other widgets, this one is not created via YWidgetFactory
 * or YOptionalWidgetFactory but with YApplication::openContextMenu()
 **/
class YContextMenu : public YSelectionWidget
{
protected:
    /**
     * Constructor.
     *
     * 'label' is the user-visible text on the button (not above it like all
     * other SelectionWidgets).
     **/
    YContextMenu();

public:
    /**
     * Destructor.
     **/
    virtual ~YContextMenu();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YContextMenu"; }

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * The application should call this (once) after all items have been added
     * with addItem(). YContextMenu::addItems() calls this automatically.
     *
     * Derived classes are required to implement this.
     **/
    virtual void rebuildMenuTree() = 0;

    /**
     * Add multiple items. For some UIs, this can be more efficient than
     * calling addItem() multiple times. This function also automatically calls
     * resolveShortcutConflicts() and rebuildMenuTree() at the end.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function at the end of the new implementation.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

    /**
     * Add one item. This widget assumes ownership of the item object and will
     * delete it in its destructor.
     *
     * This reimplementation will an index to the item that is unique for all
     * items in this ContextMenu. That index can be used later with
     * findMenuItem() to find the item by that index.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item_disown );

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Resolve keyboard shortcut conflicts: Change shortcuts of menu items if
     * there are duplicates in the respective menu level.
     *
     * This has to be called after all items are added, but before rebuildMenuTree()
     * (see above). YContextMenu::addItems() calls this automatically.
     **/
    void resolveShortcutConflicts();

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This function may throw YUIPropertyExceptions.
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string    & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw YUIPropertyExceptions.
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

protected:

    /**
     * Recursively find the first menu item with the specified index.
     * Returns 0 if there is no such item.
     **/
    YMenuItem * findMenuItem( int index );

    /**
     * Recursively find the first menu item with the specified index
     * from iterator 'begin' to iterator 'end'.
     *
     * Returns 0 if there is no such item.
     **/
    YMenuItem * findMenuItem( int index, YItemConstIterator begin, YItemConstIterator end );

    /**
     * Alias for findMenuItem(). Reimplemented to ensure consistent behaviour
     * with YSelectionWidget::itemAt().
     **/
    YMenuItem * itemAt( int index )
	{ return findMenuItem( index ); }

private:

    /**
     * Assign a unique index to all items from iterator 'begin' to iterator 'end'.
     **/
    void assignUniqueIndex( YItemIterator begin, YItemIterator end );


    ImplPtr<YContextMenuPrivate> priv;
};


#endif // YContextMenu_h
