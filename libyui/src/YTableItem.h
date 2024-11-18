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

  File:         YTableItem.h

  Author:       Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YTableItem_h
#define YTableItem_h

#include "YTreeItem.h"


class YTableCell;

// without "documenting" the file, typedefs will be dropped
//! @file

//! Collection of pointers to YTableCell
typedef std::vector<YTableCell *>		YTableCellCollection;

//! Mutable iterator over @ref YTableCellCollection
typedef YTableCellCollection::iterator		YTableCellIterator;

//! Const   iterator over @ref YTableCellCollection
typedef YTableCellCollection::const_iterator	YTableCellConstIterator;



/**
 * Item class for YTable items. Each YTableItem corresponds to one row in a
 * YTable.
 *
 * A YTableItem might have any number of cells (columns within this row),
 * including none. The YTable widget is free to ignore any excess cells if
 * there are more than the YTable widget has columns. The YTable widget is to
 * treat nonexistent cells like empty ones.
 *
 * Note that while YTable items and their cells can be manipulated through
 * pointers, their visual representation on screen might be updated only upon
 * calling certain methods of the YTable widget. See the YTable reference for
 * details.
 **/
class YTableItem: public YTreeItem
{
public:

    /**
     * Default constructor. Use addCell() to give it any content.
     **/
    YTableItem();

    /**
     * Constructor for a nested table item, i.e. one with a parent item.
     **/
    YTableItem( YTableItem * parent,
                bool         isOpen = false );

    /**
     * Convenience constructor for a (toplevel) table item without any icons.
     *
     * This will create up to 10 (0..9) cells. Empty cells for empty labels at
     * the end of the labels are not created, but empty cells in between are.
     *
     *     new YTableItem( "one", "two", "", "", "five" );
     *
     * will create an item with 5 cells:
     *
     *     cell[0] ==> "one"
     *     cell[1] ==> "two"
     *     cell[2] ==> ""
     *     cell[3] ==> ""
     *     cell[4] ==> "five"
     **/
    YTableItem( const std::string & label_0,
                const std::string & label_1 = std::string(),
                const std::string & label_2 = std::string(),
                const std::string & label_3 = std::string(),
                const std::string & label_4 = std::string(),
                const std::string & label_5 = std::string(),
                const std::string & label_6 = std::string(),
                const std::string & label_7 = std::string(),
                const std::string & label_8 = std::string(),
                const std::string & label_9 = std::string() );

    /**
     * Convenience constructor for a nested table item without any icons.
     **/
    YTableItem( YTableItem *        parent,
                const std::string & label_0,
                const std::string & label_1 = std::string(),
                const std::string & label_2 = std::string(),
                const std::string & label_3 = std::string(),
                const std::string & label_4 = std::string(),
                const std::string & label_5 = std::string(),
                const std::string & label_6 = std::string(),
                const std::string & label_7 = std::string(),
                const std::string & label_8 = std::string(),
                const std::string & label_9 = std::string() );

    /**
     * Destructor.
     *
     * This will delete all cells.
     **/
    virtual ~YTableItem();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * itemClass() const { return "YTableItem"; }

    /**
     * Add a cell. This item will assume ownership over the cell and delete it
     * when appropriate (when the table is destroyed or when table items are
     * replaced), at which time the pointer will become invalid.
     *
     * Cells can still be changed after they (and the item they belong to) are
     * added, but in that case, YTable::cellChanged() needs to be called to
     * update the table display accordingly.
     **/
    void addCell( YTableCell * cell_disown );

    /**
     * Create a new cell and add it (even if all 'label', 'iconName' and
     * 'sortKey' are empty).
     **/
    void addCell( const std::string & label,
                  const std::string & iconName = std::string(),
		  const std::string & sortKey  = std::string() );

    /**
     * Add up to 10 cells without any icons.
     **/
    void addCells( const std::string & label_0,
                   const std::string & label_1,
                   const std::string & label_2 = std::string(),
                   const std::string & label_3 = std::string(),
                   const std::string & label_4 = std::string(),
                   const std::string & label_5 = std::string(),
                   const std::string & label_6 = std::string(),
                   const std::string & label_7 = std::string(),
                   const std::string & label_8 = std::string(),
                   const std::string & label_9 = std::string() );

    /**
     * Delete all cells.
     **/
    void deleteCells();

    /**
     * Return an iterator that points to the first cell of this item.
     **/
    YTableCellIterator          cellsBegin()            { return _cells.begin(); }
    YTableCellConstIterator     cellsBegin() const      { return _cells.begin(); }

    /**
     * Return an iterator that points after the last cell of this item.
     **/
    YTableCellIterator          cellsEnd()              { return _cells.end(); }
    YTableCellConstIterator     cellsEnd() const        { return _cells.end(); }

    /**
     * Return the cell at the specified index (counting from 0 on)
     * or 0 if there is none.
     **/
    const YTableCell * cell( int index ) const;
    YTableCell *       cell( int index );

    /**
     * Return the number of cells this item has.
     **/
    int cellCount() const { return _cells.size(); }

    /**
     * Return 'true' if this item has a cell with the specified index
     * (counting from 0 on), 'false' otherwise.
     **/
    bool hasCell( int index ) const;

    /**
     * Return the label of cell no. 'index' (counting from 0 on) or an empty
     * string if there is no cell with that index.
     **/
    std::string label( int index ) const;

    /**
     * Return the icon name of cell no. 'index' (counting from 0 on) or an empty
     * string if there is no cell with that index.
     **/
    std::string iconName( int index ) const;

    /**
     * Return 'true' if there is a cell with the specified index that has an
     * icon name.
     **/
    bool hasIconName( int index ) const;

    /**
     * Just for debugging.
     **/
    std::string label() const { return label(0); }

    /**
     * Return a descriptive label of this item instance for debugging.
     * This might be truncated if the original label is too long.
     **/
    virtual std::string debugLabel() const;


private:

    // Disable unwanted base class methods. They don't make sense in this
    // context since there is not just one single label or icon name, but one
    // for each cell.

    std::string	iconName()	const			{ return ""; }
    bool	hasIconName()	const			{ return false; }
    void	setLabel	( const std::string & )	{}
    void	setIconName	( const std::string & )	{}


    //
    // Data members
    //

    YTableCellCollection _cells;
};



/**
 * One cell (one column in one row) of a YTableItem. Each cell has a label (a
 * user visible text), optionally an icon (*) and also optionally a sort-key.
 *
 * Note that cells don't have individual IDs; they have just an index.
 * The first cell in an item is cell(0). In an ideal world, each YTableItem
 * would have exactly as many cells as there are columns in the YTable, but
 * these classes make no such assumptions. A YTableItem might have any number
 * of cells, including none.
 *
 * The YTable widget is free to ignore any excess cells if there are more than
 * the YTable widget has columns. If there are less cells than the table has
 * columns, the nonexistent cells will be treated as empty.
 *
 *
 * (*) Not all UIs can handle icons. UIs that can't handle them will simply
 * ignore any icons specified for YTableCells. Thus, applications should either
 * check the UI capabilities if it can handle icons or use icons only as an
 * additional visual cue that still has a text counterpart (so the user can
 * still make sense of the table content when no icons are visible).
 **/
class YTableCell
{
public:
    /**
     * Constructor with label and optional icon name and optional sort
     * key for cells that don't have a parent item yet (that will be
     * added to a parent later with setParent()).
     **/
    YTableCell( const std::string & label, const std::string & iconName = "",
		const std::string & sortKey = "" )
        : _label( label )
        , _iconName( iconName )
	, _sortKey( sortKey )
	, _parent( 0 )
	, _column ( -1 )
        {}

    /**
     * Constructor with parent, column no., label and optional icon name for
     * cells that are created with a parent.
     **/
    YTableCell( YTableItem *		parent,
		int			column,
		const std::string &	label,
		const std::string &	iconName = "",
		const std::string &     sortKey = "" )
        : _label( label )
        , _iconName( iconName )
	, _sortKey( sortKey )
	, _parent( parent )
	, _column ( column )
        {}

    /**
     * Destructor. Not strictly needed inside this class, but useful for
     * derived classes. Since this is the only virtual method of this class,
     * the cost of this is a vtable for this class and a pointer to the vtable
     * in each instance.
     **/
    virtual ~YTableCell() {}

    /**
     * Return this cells's label. This is what the user sees in a dialog, so
     * this will usually be a translated text.
     **/
    std::string label() const { return _label; }

    /**
     * Set this cell's label.
     *
     * If this is called after the corresponding table item (table row) is
     * added to the table widget, call YTable::cellChanged() to notify the
     * table widget about the fact. Only then will the display be updated.
     **/
    void setLabel( const std::string & newLabel ) { _label = newLabel; }

    /**
     * Return this cell's icon name.
     **/
    std::string iconName() const { return _iconName; }

    /**
     * Return 'true' if this cell has an icon name.
     **/
    bool hasIconName() const { return ! _iconName.empty(); }

    /**
     * Set this cell's icon name.
     *
     * If this is called after the corresponding table item (table row) is
     * added to the table widget, call YTable::cellChanged() to notify the
     * table widget about the fact. Only then will the display be updated.
     **/
    void setIconName( const std::string & newIconName ) { _iconName = newIconName; }

    /**
     * Return this cell's sort key.
     **/
    std::string sortKey() const { return _sortKey; }

    /**
     * Return 'true' if this cell has a sort key.
     **/
    bool hasSortKey() const { return ! _sortKey.empty(); }

    /**
     * Set this cell's sort key.
     *
     * If this is called after the corresponding table item (table row) is
     * added to the table widget, call YTable::cellChanged() to notify the
     * table widget about the fact. Only then will the display be updated.
     **/
    void setSortKey( const std::string & newSortKey ) { _sortKey = newSortKey; }

    /**
     * Return this cell's parent item or 0 if it doesn't have one yet.
     **/
    YTableItem * parent() const { return _parent; }

    /**
     * Return this cell's column no. (counting from 0on) or -1 if it doesn't
     * have a parent yet.
     **/
    int column() const { return _column; }

    /**
     * Convenience function: Return this cell's parent item's index within its
     * table widget or -1 if there is no parent item or no parent table.
     **/
    int itemIndex() const { return _parent ? _parent->index() : -1; }

    /**
     * Set this cell's parent item and column no. if it doesn't have a parent
     * yet.
     *
     * This method will throw an exception if the cell already has a parent.
     **/
    void reparent( YTableItem * parent, int column );


private:

    std::string		_label;
    std::string		_iconName;
    std::string         _sortKey;
    YTableItem *	_parent;
    int			_column;
};


#endif // YTableItem_h
