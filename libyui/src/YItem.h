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

  File:		YItem.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YItem_h
#define YItem_h

#include <string>
#include <vector>
#include <iosfwd>


class YItem;

// without "documenting" the file, typedefs will be dropped
//! @file

//! Collection of pointers to YItem.
typedef std::vector<YItem *> 			YItemCollection;

//! Mutable iterator over @ref YItemCollection.
typedef YItemCollection::iterator		YItemIterator;

//! Const   iterator over @ref YItemCollection.
typedef YItemCollection::const_iterator		YItemConstIterator;


/**
 * Simple item class for SelectionBox, ComboBox, MultiSelectionBox etc. items.
 * This class provides stubs for children management.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 **/
class YItem
{
public:
    /**
     * Constructor with just the label and optionally the selected state.
     **/
    YItem( const std::string & label,
           bool                selected = false )
	: _label( label )
	, _status( selected ? 1 : 0 )
	, _index( -1 )
	, _data( 0 )
	{}

    /**
     * Constructor with label and icon name and optionally the selected state.
     **/
    YItem( const std::string & label,
           const std::string & iconName,
           bool                selected = false )
	: _label( label )
	, _iconName( iconName )
	, _status( selected ? 1 : 0 )
	, _index( -1 )
	, _data( 0 )
	{}

    /**
     * Destructor.
     **/
    virtual ~YItem() {}

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * itemClass() const { return "YItem"; }

    /**
     * Return this item's label. This is what the user sees in a dialog, so
     * this will usually be a translated text.
     **/
    std::string label() const { return _label; }

    /**
     * Set this item's label.
     **/
    void setLabel( const std::string & newLabel ) { _label = newLabel; }

    /**
     * Return this item's icon name.
     **/
    std::string iconName() const { return _iconName; }

    /**
     * Return 'true' if this item has an icon name.
     **/
    bool hasIconName() const { return ! _iconName.empty(); }

    /**
     * Set this item's icon name.
     **/
    void setIconName( const std::string & newIconName ) { _iconName = newIconName; }

    /**
     * Return 'true' if this item is currently selected.
     **/
    bool selected() const { return _status != 0; }

    /**
     * Select or unselect this item. This does not have any effect on any other
     * item; if it is desired that only one item is selected at any time, the
     * caller has to take care of that.
     **/
    void setSelected( bool sel = true ) { _status = sel ? 1 : 0; }

    /**
     * Return the status of this item. This is a bit more generalized than
     * 'selected'. Values other than 0 or 1 can mean different things to the
     * application or to the specific widget.
     **/
    int status() const { return _status; }

    /**
     * Set the status of this item. Most widgets only use 0 for "not selected"
     * or nonzero for "selected". Some widgets may make use of other values as
     * well.
     **/
    void setStatus( int newStatus ) { _status = newStatus; }

    /**
     * Set this item's index.
     **/
    void setIndex( int index ) { _index = index; }

    /**
     * Return the index of this item (as set with setIndex() ).
     **/
    int index() const { return _index; }

    /**
     * Set the opaque data pointer for application use.
     *
     * Applications can use this to store the pointer to a counterpart of this
     * tree item. It is the application's responsibility to watch for dangling
     * pointers and possibliy deleting the data. All this class ever does with
     * this pointer is to store it.
     **/
    void setData( void * newData ) { _data = newData; }

    /**
     * Return the opaque data pointer.
     **/
    void * data() const { return _data; }

    //
    // Children management stubs.
    //
    // Derived classes that can handle child items should reimplement those
    // functions.
    // The following default implementations don't do anything with children;
    // they act as if this item didn't have any children.
    //

    /**
     * Return 'true' if this item has any child items.
     **/
    virtual bool hasChildren() const { return false; }

    /**
     * Return an iterator that points to the first child item of this item.
     *
     * This default implementation returns the 'end' iterator of the
     * class-static always empty _noChildren YItemCollection.
     * It is safe to use this iterator in classic iterator loops:
     *
     *   for ( YItemIterator it = myItem->childrenBegin();
     *         it != myItem->childrenEnd();
     *         ++it )
     *	 {
     *       ...
     *   }
     *
     * The loop body will only ever be executed if this item is a derived class
     * that actually manages child items.
     **/
    virtual YItemIterator	childrenBegin()		{ return _noChildren.end(); }
    virtual YItemConstIterator	childrenBegin() const	{ return _noChildren.end(); }

    /**
     * Return an iterator that points after the last child item of this item.
     *
     * This default implementation returns the 'end' iterator of the
     * class-static always empty _noChildren YItemCollection.
     **/
    virtual YItemIterator	childrenEnd() 		{ return _noChildren.end(); }
    virtual YItemConstIterator	childrenEnd() const	{ return _noChildren.end(); }

    /**
     * Return this item's parent item or 0 if it is a toplevel item.
     * This default implementation always returns 0.
     * Derived classes that handle children should reimplement this.
     **/
    virtual YItem * parent() const { return 0; }

    /**
     * Return a descriptive label of this item instance for debugging.
     * This might be truncated if the original label is too long.
     **/
    virtual std::string debugLabel() const;

    /**
     * Return a string of maximum 'limit' characters. Add an ellipsis ("...")
     * if it was truncated.
     **/
    std::string limitLength( const std::string & text, int limit ) const;


private:

    std::string	_label;
    std::string	_iconName;
    int         _status;
    int		_index;
    void *	_data;

    /**
     * Static children collection that is always empty so the children
     * iterators of this base class have something valid to return.
     **/
    static YItemCollection _noChildren;
};


std::ostream & operator<<( std::ostream & stream, const YItem * item );



#endif // YItem_h
