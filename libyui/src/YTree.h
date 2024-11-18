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

  File:		YTree.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YTree_h
#define YTree_h

#include "YSelectionWidget.h"

class YTreeItem;
class YTreePrivate;


/**
 * Tree: List box that displays a (scrollable) list of hierarchical items from
 * which the user can select exactly one. Each item has a label text and an
 * optional icon (*).
 *
 * This is very similar to SelectionBox, but each item can have subitems that
 * can be open (expanded) or closed (collapsed).
 *
 * The tree widget also has a caption label that is displayed above the
 * tree. The hotkey displayed in that caption label will move the keyboard
 * focus into the tree item list.
 *
 *
 * (*) Not all UIs (in particular not text-based UIs) support displaying icons,
 * so an icon should never be an exclusive means to display any kind of
 * information.

 * 'multiSelection' indicates whether or not the user can select multiple
 * items at the same time. This can only be set in the constructor.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 **/
class YTree : public YSelectionWidget
{
protected:

    /**
     * Constructor.
     **/
    YTree( YWidget *           parent,
           const std::string & label,
           bool                multiSelection,
           bool                recursiveSelection);

public:

    /**
     * Destructor.
     **/
    virtual ~YTree();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YTree"; }

    /**
     * Rebuild the displayed tree from the internally stored YTreeItems.
     *
     * The application should call this (once) after all items have been added
     * with addItem(). YTree::addItems() calls this automatically.
     *
     * Derived classes are required to implement this.
     **/
    virtual void rebuildTree() = 0;

    /**
     * Add multiple items. For some UIs, this can be more efficient than
     * calling addItem() multiple times. This function also automatically calls
     * rebuildTree() at the end.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function at the end of the new implementation.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

    /**
     * Deliver even more events than with notify() set.
     *
     * For YTree, this is relevant mostly for the NCurses UI:
     *
     * In graphical UIs like the Qt UI, the user can use the mouse to select an
     * item in a tree. With notify() set, this will send an event right away
     * (i.e., it will make UserInput and related return, while normally it
     * would only return when the user clicks a PushButton).
     *
     * In the NCurses UI, there is no mouse, so the user has to use the cursor
     * keys to move to the item he wants to select. In immediateMode(), every
     * cursor key press will make the tree send an event. Without
     * immediateMode(), the NCTree will wait until the user hits the [Return]
     * key until an event is sent. Depending on what the application does upon
     * each selection box event, immediateMode() might make the application
     * less responsive.
     **/
    bool immediateMode() const;

    /**
     * Set immediateMode() on or off.
     **/
    void setImmediateMode( bool on = true );

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

    /**
     * The name of the widget property that will return user input.
     * Inherited from YWidget.
     **/
    const char * userInputProperty() { return YUIProperty_CurrentItem; }


    /**
     * Return 'true' if the user can select multiple items  at the same time
     **/
    bool hasMultiSelection() const;

    /**
     * Return the the item that currently has the keyboard focus
     * or 0 if no item currently has the keyboard focus.
     *
     * Notice that for a MultiSelectionBox the current item is not necessarily
     * selected, i.e., its check box may or may not be checked.
     *
     * Derived classes are required to implement this function.
     **/
    virtual YTreeItem * currentItem() = 0;

    /**
    * Activate the item selected in the tree. This can be used in automated
    * tests to simulate user input.
    *
    * Derived classes are required to implement this.
    **/
    virtual void activate() = 0;

    /**
     * Return the item in the tree that matches path of labels or 0 if not found.
     *
     * 'path' is a vector of strings with the path components, e.g.
     * ["usr", "share", "doc", "packages"].
     **/
    YTreeItem * findItem( const std::vector<std::string> & path ) const;


protected:

    /**
     * Recursively search the items between item iterators 'begin' and 'end'
     * for a path specified in a string vector between 'path_begin' and
     * 'path_end'. Return that item or 0 if not found.
     *
     * This is a helper function for findItem( std::vector<std::string> & ).
     */
    YTreeItem * findItem( std::vector<std::string>::const_iterator path_begin,
                          std::vector<std::string>::const_iterator path_end,
                          YItemConstIterator                       begin,
                          YItemConstIterator                       end ) const;

private:

    ImplPtr<YTreePrivate> priv;
};


#endif // YTree_h
