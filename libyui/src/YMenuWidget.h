/*
  Copyright (c) [2020] SUSE LLC
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

  File:		YMenuWidget.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMenuWidget_h
#define YMenuWidget_h

#include "YSelectionWidget.h"
#include "YMenuItem.h"

class YMenuWidgetPrivate;


/**
 * Abstract base class for widgets that handle menus, e.g. YMenuButton or
 * YMenuBar.
 **/
class YMenuWidget: public YSelectionWidget
{
protected:
    /**
     * Constructor.
     *
     * 'label' is only passed through to the inherited class. For a menu
     * button, this would be the label on the button; a menu bar does not have
     * a label.
     **/
    YMenuWidget( YWidget * parent, const std::string & label = "" );

public:

    /**
     * Destructor.
     **/
    virtual ~YMenuWidget();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMenuWidget"; }

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * The application should call this (once) after all items have been added
     * with addItem(). YMenuButton::addItems() calls this automatically.
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
     * This reimplementation will assign an index to the item that is unique
     * for all items in this MenuButton. That index can be used later with
     * findMenuItem() to find the item by that index.
     *
     * @note Do not forget to call #resolveShortcutConflicts and
     * #rebuildMenuTree (in this order!) after adding all elements.
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
     * This has to be called after all items are added, but before
     * rebuildMenuTree() (see above). YMenuWidget::addItems() calls this
     * automatically.
     **/
    void resolveShortcutConflicts();

    /**
     * Enable or disable an item. This default implementation only updates the
     * item's 'enabled' field.
     *
     * Derived classes should overwrite this method and either update the
     * item's 'enabled' field in their implementation or call this default
     * implementation.
     **/
    virtual void setItemEnabled( YMenuItem * item, bool enabled );

    /**
     * Show or hide an item. This default implementation only updates the
     * item's 'visible' field.
     *
     * Derived classes should overwrite this method and either update the
     * item's 'visible' field in their implementation or call this default
     * implementation.
     **/
    virtual void setItemVisible( YMenuItem * item, bool visible );

    /**
     * Support for the Rest API for UI testing:
     *
     * Return the item in the tree which matches a path of labels. This
     * returns 0 if there is no such item or if it is not a leaf menu item.
     *
     * 'path' specifies the user-visible labels (i.e. the translated texts) of
     * each menu level ( ["File", "Export", "As XML"] ).
     **/
    YMenuItem * findItem( std::vector<std::string> & path ) const;

    /**
     * Support for the Rest API for UI testing:
     *
     * Activate the item selected in the tree.
     * This can be used in tests to simulate user input.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activateItem( YMenuItem * item ) = 0;

    /**
     * Recursively find the first menu item with the specified index.
     * Returns 0 if there is no such item.
     **/
    YMenuItem * findMenuItem( int index );


    // No propertySet(), setProperty(), getProperty() on this level:
    //
    // This is left to derived widgets (very much like for YSelectionWidget)
    // so they can be tailored to what they really can provide.
    //
    // There is infrastructure for properties like "EnabledItems" here and in
    // the UI bindings, though, to easily support those properties.

protected:

    /**
     * Resolve keyboard shortcut conflicts between iterators 'begin' and 'end'.
     **/
    void resolveShortcutConflicts( YItemConstIterator begin,
                                   YItemConstIterator end );

    /**
     * Recursively find the first menu item with the specified index
     * from iterator 'begin' to iterator 'end'.
     *
     * Returns 0 if there is no such item.
     **/
    YMenuItem * findMenuItem( int                index,
                              YItemConstIterator begin,
                              YItemConstIterator end );

    /**
     * Recursively looks for the first item in the tree of the menu items
     * using depth first search.
     * Return nullptr if item which matches full path is not found.
     * Path is a vector of strings, where next element is a child item, so
     * in case one needs to select File->Export->As PDF, for instance,
     * Vector will look like [ "File", "Export", "As PDF" ].
     */
    YMenuItem * findItem( std::vector<std::string>::iterator path_begin,
                          std::vector<std::string>::iterator path_end,
                          YItemConstIterator                 begin,
                          YItemConstIterator                 end ) const;

    /**
     * Alias for findMenuItem(). Reimplemented to ensure consistent behaviour
     * with YSelectionWidget::itemAt().
     **/
    YMenuItem * itemAt( int index )
	{ return findMenuItem( index ); }

    /**
     * Assign a unique index to all items from iterator 'begin' to iterator
     * 'end'.
     **/
    void assignUniqueIndex( YItemIterator begin, YItemIterator end );


private:

    ImplPtr<YMenuWidgetPrivate> priv;
};


#endif  // YMenuWidget_h
