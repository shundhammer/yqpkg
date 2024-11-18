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

  File:	      YQTable.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQTable_h
#define YQTable_h

#include <QFrame>
#include "QY2ListView.h"
#include <yui/YTable.h>


class QY2ListView;
class QTreeWidgetItem;
class YQTableListViewItem;


class YQTable : public QFrame, public YTable
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQTable( YWidget *          parent,
             YTableHeader *     header,
             bool               multiSelection );

    /**
     * Destructor.
     **/
    virtual ~YQTable();

    /**
     * Switch between sorting by item insertion order (keepSorting: true) or
     * allowing the user to sort by an arbitrary column (by clicking on the
     * column header).
     *
     * Reimplemented from YTable.
     **/
    virtual void setKeepSorting( bool keepSorting );

    /**
     * Add an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Add multiple items.
     *
     * Reimplemented for efficiency from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

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
     * Notification that a cell (its text and/or its icon) was changed from the
     * outside. Applications are required to call this whenever a table cell is
     * changed after adding the corresponding table item (the row) to the table
     * widget.
     *
     * Reimplemented from YTable.
     **/
    virtual void cellChanged( const YTableCell * cell );

    /**
     * Set enabled/disabled state.
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
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();


protected slots:

    /**
     * Notification that an item is selected (single click or keyboard).
     **/
    void slotSelected( QTreeWidgetItem * );

    /**
     * Notification that the item selection changed
     * (relevant for multiSelection mode).
     **/
    void slotSelectionChanged();

    /**
     * Notification that an item is activated (double click or keyboard).
     **/
    void slotActivated( QTreeWidgetItem * );

    /**
     * Propagate an "item expanded" event to the underlying YTableItem.
     **/
    void slotItemExpanded( QTreeWidgetItem * item );

    /**
     * Propagate an "item collapsed" event to the underlying YTableItem.
     **/
    void slotItemCollapsed( QTreeWidgetItem * item );


    /**
     * Propagate a context menu selection.
     *
     * This will trigger a 'ContextMenuActivated' event if 'notifyContextMenu' is set.
     **/
    void slotContextMenu ( const QPoint & pos );



protected:

    /**
     * Select the original item (the YTableItem) that corresponds to the
     * specified listViewItem.
     **/
    void selectOrigItem( QTreeWidgetItem * listViewItem );

    /**
     * Internal addItem() method that will not do expensive operations in batch
     * mode.
     **/
    void addItem( YItem * item, bool batchMode, bool resizeColumnsToContent );

    /**
     * Clone (create Qt item counterparts) for all child items of 'parentItem'.
     * Set their Qt item parent to 'parentItemClone'.
     **/
    void cloneChildItems( YTableItem          * parentItem,
                          YQTableListViewItem * parentItemClone );

    //
    // Data members
    //

    QY2ListView * _qt_listView;
};



/**
 * Visual representation of a YTableItem.
 **/
class YQTableListViewItem : public QY2ListViewItem
{
public:

    /**
     * Constructor for toplevel items.
     **/
    YQTableListViewItem( YQTable     * table,
			 QY2ListView * parent,
			 YTableItem  * origItem );

    /**
     * Constructor for nested items.
     **/
    YQTableListViewItem( YQTable             * table,
			 YQTableListViewItem * parentItemClone,
			 YTableItem          * origItem );

    /**
     * Return the parent table widget.
     **/
    YQTable * table() const { return _table; }

    /**
     * Return the corresponding YTableItem.
     **/
    YTableItem * origItem() const { return _origItem; }

    /**
     * Update this item's display with the content of 'cell'.
     **/
    void updateCell( const YTableCell * cell );

    /**
     * Update all columns of this item with the content of the original item.
     **/
    void updateCells();

    /**
     * The text of the table cell or the sort-key if available.
     **/
    virtual QString smartSortKey(int column) const override;

protected:

    /**
     * Common initializations for all constructors
     **/
    void init();

    /**
     * Set the alignment for each column according to the YTable parent's
     * alignment.
     **/
    void setColAlignment();


    YQTable *	 _table;
    YTableItem * _origItem;
};



#endif // YQLabel_h
