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

  File:		YSelectionWidget.h

  Author:     	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSelectionWidget_h
#define YSelectionWidget_h

#include "YWidget.h"
#include "YItem.h"
#include "ImplPtr.h"

class YSelectionWidgetPrivate;

/**
 * Base class for various kinds of multi-value widgets.
 *   - YSelectionBox, YMultiSelectionBox, YComboBox
 *   - YContextMenu, YMenuButton
 *   - YTable
 *   - YTree
 *   - YDumbTab
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 **/
class YSelectionWidget : public YWidget
{
protected:

    /**
     * Constructor.
     *
     * 'enforceSingleSelection' indicates if this base class should enforce
     * single selection when items are added or when items are selected from
     * the application. Note that single selection can also mean that no item
     * is selected.
     **/
    YSelectionWidget( YWidget *			parent,
		      const std::string &	label,
		      bool			enforceSingleSelection,
		      bool			recursiveSelection = false );

public:
    /**
     * Destructor.
     **/
    virtual ~YSelectionWidget();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YSelectionWidget"; }

    /**
     * Return this widget's label (the caption above the item list).
     **/
    std::string label() const;

    /**
     * Change this widget's label (the caption above the item list).
     *
     * Derived classes should overwrite this function, but they should call
     * this base class function in the new implementation.
     **/
    virtual void setLabel( const std::string & newLabel );

    /**
     * Add one item. This widget assumes ownership of the item object and will
     * delete it in its destructor.
     *
     * NOTE: For tree items, call this only for the toplevel items; all
     * non-toplevel items are already owned by their respective parent
     * items. Adding them to the parent widget will clash with this ownership.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function in the new implementation.
     **/
    virtual void addItem( YItem * item_disown );

    /**
     * Overloaded for convenience: Add an item by string.
     **/
    void addItem( const std::string & itemLabel, bool selected = false );

    /**
     * Overloaded for convenience: Add an item with a text and an icon.
     * Note that not all UIs can display icons.
     **/
    void addItem( const std::string & itemLabel,
		  const std::string & iconName,
		  bool                selected = false );

    /**
     * Add multiple items. For some UIs, this can be more efficient than
     * calling addItem() multiple times.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

    /**
     * Delete all items.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function in the new implementation.
     **/
    virtual void deleteAllItems();

    /**
     * Delete all items and add new items.
     **/
    void setItems( const YItemCollection & itemCollection )
	{ deleteAllItems(); addItems( itemCollection ); }

    /**
     * Return an iterator that points to the first item.
     *
     * For YSelectionWidgets that can have tree structures, this iterator will
     * iterate over the toplevel items.
     *
     * Important: Don't use this iterator to iterate over all items and check
     * their "selected" state; that information might not always be up to
     * date. Use the dedicated functions for that.
     **/
    YItemIterator	itemsBegin();
    YItemConstIterator	itemsBegin() const;

    /**
     * Return an iterator that points behind the last item.
     **/
    YItemIterator	itemsEnd();
    YItemConstIterator	itemsEnd() const;

    /**
     * Return 'true' if this widget has any items.
     **/
    bool hasItems() const;

    /**
     * Return the number of items.
     *
     * For YSelectionWidgets that can have tree structures, this returns the
     * number of toplevel items.
     **/
    int itemsCount() const;

    /**
     * Return the item at index 'index' (from 0)
     * or 0 if there is no such item.
     **/
    YItem * itemAt( int index ) const;

    /**
     * Return the first item or 0 if there is none.
     **/
    YItem * firstItem() const;

    /**
     * Return the (first) selected item or 0 if none is selected.
     **/
    virtual YItem * selectedItem();

    /**
     * Return all selected items. This is mostly useful for derived classes
     * that allow selecting multiple items.
     *
     * This function does not transfer ownership of those items to the caller,
     * so don't try to delete them!
     **/
    virtual YItemCollection selectedItems();

    /**
     * Return 'true' if any item is selected.
     **/
    bool hasSelectedItem();

    /**
     * Select or deselect an item.
     *
     * Notice that this is different from YItem::setSelected() because unlike
     * the latter function, this function informs the parent widget of the
     * selection change.
     *
     * If only one item can be selected at any time (single selection), the
     * derived class will make sure to deselect any previous selection, if
     * applicable.
     *
     * Derived classes should overwrite this function, but they should call
     * this base class function at the new function's start (this will also
     * check if the item really belongs to this widget and throw an exception
     * if not).
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Set the status of an item.
     *
     * This is similar to selectItem(), but with numeric values.
     *
     * This default implementation just calls selectItem() with 'status'
     * converted to boolean. Derived classes can choose to make more detailed
     * use of the numeric value.
     **/
    virtual void setItemStatus( YItem * item, int status );

    /**
     * Deselect all items.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function in the new implementation.
     **/
    virtual void deselectAllItems();

    /**
     * Set this widget's base path where to look up icons.
     * If this is a relative path, YUI::qApp()->iconBasePath() is prepended.
     **/
    void setIconBasePath( const std::string & basePath );

    /**
     * Return this widget's base path where to look up icons
     * as set with setIconBasePath().
     **/
    std::string iconBasePath() const;

    /**
     * Return the full path + file name for the specified icon name.
     * If iconBasePath is non-empty, it is prepended to the icon name.
     * Otherwise, YUI::yApp()->iconLoader() and its icon search paths
     * is used find the icon in one of them
     *
     * If 'iconName' is empty, this will return an empty string.
     **/
    std::string iconFullPath( const std::string & iconName ) const;

    /**
     * Return the full path + file name for the icon of the specified item.
     * If iconBasePath is non-empty, it is prepended to the item's iconName.
     * Otherwise, YUI::yApp()->iconLoader() and its icon search paths
     * is used find the icon in one of them
     *
     * If 'item' does not have an iconName specified, this will return an empty
     * string.
     **/
    std::string iconFullPath( YItem * item ) const;

    /**
     * Return 'true' if this widget's items contain the specified item.
     **/
    bool itemsContain( YItem * item ) const;

    /**
     * Find the (first) item with the specified label.
     * Return 0 if there is no item with that label.
     **/
    YItem * findItem( const std::string & itemLabel ) const;

    /**
     * Dump all items and their selection state to the log.
     **/
    void dumpItems() const;

    /**
     * Return 'true' if this base class should enforce single selection.
     **/
    bool enforceSingleSelection() const;

    /**
     * Notification that any shortcut of any item was changed by the shortcut
     * conflict manager YShortcutManager.
     *
     * Derived classes should reimplement this.
     **/
    virtual void shortcutChanged() {}

    /**
     * Get the string of this widget that holds the keyboard shortcut.
     *
     * Notice that some sub-classes (e.g., YDumbTab, YItemSelection, YMenuBar)
     * has one shortcut for each item. This value is not meaningful for such
     * widget classes.
     *
     * Check YItemShortcut in YShortcut.{cc,h} for more details.
     *
     * Reimplemented from YWidget.
     **/
    virtual std::string shortcutString() const { return label(); }

    /**
     * Set the string of this widget that holds the keyboard shortcut.
     *
     * Also trigger a shortcutChanged() notification. This is useful for
     * derived sub-classes to refresh the widget when any shortcut of any item
     * was changed by the shortcut conflict manager.
     *
     * Check YItemShortcut in YShortcut.{cc,h} for more details.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setShortcutString( const std::string & str );


protected:

    /**
     * Set single selection mode on or off. In single selection mode, only one
     * item can be selected at any time.
     *
     * If set, this base class enforces this when items are added or when items
     * are selected from the application. Note that single selection can also
     * mean that no item is selected.
     **/
    void setEnforceSingleSelection( bool on );

    /**
     * In single selection mode, enforce selecting an initial item
     * ('true' by default). This is ignored in multi selection mode.
     **/
    void setEnforceInitialSelection( bool on );

    /**
     * Return 'true' if this class enforces an initial selection.
     **/
    bool enforceInitialSelection() const;

    /**
     * Return 'true' if this base class should select children recursively.
     **/
    bool recursiveSelection() const;

    /**
     * Recursively try to find the first selected item between iterators
     * 'begin' and 'end'. Return that item or 0 if there is none.
     **/
    YItem * findSelectedItem( YItemConstIterator begin,
			      YItemConstIterator end );

    /**
     * Recursively find all selected items between iterators 'begin' and 'end'
     * and add each of them to the 'selectedItems' YItemCollection.
     **/
    void findSelectedItems( YItemCollection &	selectedItems,
			    YItemConstIterator	begin,
			    YItemConstIterator	end );

    /**
     * Recursively deselect all items between iterators 'begin' and 'end'.
     **/
    void deselectAllItems( YItemIterator	begin,
			   YItemIterator	end );
    /**
     * Recursively try to find an item with label 'wantedItemLabel' between
     * iterators 'begin' and 'end'. Return that item or 0 if there is none.
     **/
    YItem * findItem	( const std::string &	wantedItemLabel,
			  YItemConstIterator	begin,
			  YItemConstIterator	end ) const;

    /**
     * Recursively check if 'wantedItem' is between iterators 'begin' and
     * 'end'.
     **/
    bool itemsContain	( YItem *		wantedItem,
			  YItemConstIterator	begin,
			  YItemConstIterator	end ) const;


private:

    ImplPtr<YSelectionWidgetPrivate> priv;
};


#endif // YSelectionWidget_h
