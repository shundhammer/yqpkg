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

  File:       YQTree.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQTree_h
#define YQTree_h

#include <QFrame>
#include <QTreeWidget>

#include <yui/YTree.h>


class YQWidgetCaption;
class YQTreeItem;


class YQTree : public QFrame, public YTree
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQTree( YWidget *           parent,
            const std::string & label,
            bool                multiSelection,
            bool                recursiveSelection );

    /**
     * Destructor.
     **/
    virtual ~YQTree();

    /**
     * Change the label text.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Rebuild the displayed tree from the internally stored YTreeItems.
     *
     * The application should call this (once) after all items have been added
     * with addItem(). YTree::addItems() calls this automatically.
     *
     * Implemented from YTree.
     **/
    virtual void rebuildTree();

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();


    /**
     * Set enabled/disbled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


    /**
     * Return the the item that currently has the keyboard focus
     * or 0 if no item currently has the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox.
     **/
    virtual YTreeItem * currentItem();
    
    /**
    * Activate the item selected in the tree. Can be used in tests to simulate user input.
    *
    * Derived classes are required to implement this.
    **/
    virtual void activate();


protected slots:



    /**
     * Propagate a tree item selection.
     *
     * This will trigger a 'SelectionChanged' event if 'notify' is set.
     **/
    void slotSelectionChanged();

    /**
     * Propagate a tree item change.
     *
     * This will trigger a 'ValueChanged' event if 'notify' is set.
     **/
    void slotItemChanged( QTreeWidgetItem * item);
    void slotItemClicked( QTreeWidgetItem * item, int column );

    /**
     * Propagate a double click or pressing the space key on a tree item.
     *
     * This will trigger an 'Activated' event if 'notify' is set.
     **/
    void slotActivated( QTreeWidgetItem * item);

    /**
     * Propagate an "item expanded" event to the underlying YTreeItem.
     **/
    void slotItemExpanded( QTreeWidgetItem * item );

    /**
     * Propagate an "item collapsed" event to the underlying YTreeItem.
     **/
    void slotItemCollapsed( QTreeWidgetItem * item );

    /**
     * Propagate a context menu selection
     *
     * This will trigger an 'ContextMenuActivated' event if 'notifyContextMenu' is set.
     **/
    void slotContextMenu ( const QPoint & pos );


protected:

    /**
     * Select an item via the corresponding YQTreItem.
     **/
    void selectItem( YQTreeItem * item );

    /**
     * Select or deselect an item, recursively
     **/
    void selectItem(QTreeWidgetItem * item, bool selected, bool recursive);

    /**
     * Open the branch of 'item' recursively to its toplevel item.
     **/
    void openBranch( YQTreeItem * item );

    /**
     * Build a tree of items that will be displayed (YQTreeItems) from the
     * original items between iterators 'begin' and 'end' as child items of
     * 'parentItem' (or as toplevel items if 'parentItem' is 0).
     **/
    void buildDisplayTree( YQTreeItem *		parentItem,
			   YItemIterator	begin,
			   YItemIterator 	end );

    //
    // Data members
    //

    YQWidgetCaption *	_caption;
    QTreeWidget * 	_qt_treeWidget;
    int			_nextSerialNo;
};


class YQTreeItem: public QTreeWidgetItem
{
public:

    /**
     * Constructor for a top level item.
     **/
    YQTreeItem( YQTree	*	tree,
		QTreeWidget *	parent,
		YTreeItem *	origItem,
		int		serial	);

    /**
     * Constructor for a non-top level item.
     **/
    YQTreeItem( YQTree	*	tree,
		YQTreeItem *	parent,
		YTreeItem *	origItem,
		int		serial	);

    /**
     * Returns the original YTreeItem of which this item is a clone.
     **/
    YTreeItem * origItem() { return _origItem; }

    /**
     * Open this item.
     *
     * Reimplemented from QTreeWidgetItem.
     **/
    virtual void setOpen( bool open );


private:

    /**
     * Init function. All constructors end up here.
     **/
    void init( YQTree *		tree,
	       YTreeItem *	yTreeItem,
	       int		serial );


protected:

    /**
     * Sort key of this item.
     *
     * Reimplemented from QTreeWidgetItem.
     **/
    QString key( int column, bool ascending ) const;


    //
    // Data members
    //

    YQTree *	_tree;
    YTreeItem *	_origItem;
    int 	_serialNo;

};


#endif // ifndef YQTree_h
