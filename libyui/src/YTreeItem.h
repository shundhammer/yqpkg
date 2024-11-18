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

  File:		YTreeItem.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YTreeItem_h
#define YTreeItem_h

#include "YItem.h"

/**
 * Item class for tree items.
 *
 * This class implements children management.
 **/
class YTreeItem: public YItem
{
public:
    /**
     * Constructors for toplevel items.
     **/
    YTreeItem( const std::string &	label,
	       bool			isOpen = false );

    YTreeItem( const std::string &	label,
	       const std::string &	iconName,
	       bool			isOpen = false );

    /**
     * Constructors for items that have a parent item.
     *
     * They will automatically register this item with the parent item. The
     * parent assumes ownership of this item and will delete it in its (the
     * parent's) destructor.
     **/
    YTreeItem( YTreeItem *		parent,
	       const std::string &	label,
	       bool			isOpen = false );

    YTreeItem( YTreeItem *		parent,
	       const std::string &	label,
	       const std::string &	iconName,
	       bool			isOpen = false );

    /**
     * Destructor.
     *
     * This will delete all children.
     **/
    virtual ~YTreeItem();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * itemClass() const { return "YTreeItem"; }

    /**
     * Return 'true' if this item has any child items.
     *
     * Reimplemented from YItem.
     **/
    virtual bool hasChildren() const { return ! _children.empty(); }

    /**
     * Return an iterator that points to the first child item of this item.
     *
     * Reimplemented from YItem.
     **/
    virtual YItemIterator	childrenBegin()		{ return _children.begin(); }
    virtual YItemConstIterator	childrenBegin() const	{ return _children.begin(); }

    /**
     * Return an iterator that points after the last child item of this item.
     *
     * Reimplemented from YItem.
     **/
    virtual YItemIterator 	childrenEnd() 		{ return _children.end(); }
    virtual YItemConstIterator	childrenEnd() const	{ return _children.end(); }

    /**
     * Add a child item to this item.
     *
     * Note that the constructors that accept a parent pointer will
     * automatically add themselves to their parent, so applications will
     * normally not have to call this function.
     **/
    virtual void addChild( YItem * item_disown );

    /**
     * Delete all child items.
     **/
    virtual void deleteChildren();

   /**
     * Return 'true' if this tree item should be displayed open (with its
     * children visible) by default.
     *
     * Notice that this will always return 'false' for tree items without
     * children.
     **/
    bool isOpen() const;

    /**
     * Change the 'isOpen' flag.
     **/
    void setOpen( bool open = true );
    void setClosed() { setOpen( false ); }

    /**
     * Returns this item's parent item or 0 if it is a toplevel item.
     *
     * Reimplemented from YItem.
     **/
    virtual YTreeItem * parent() const { return _parent; }

private:

    YTreeItem *		_parent;
    YItemCollection	_children;
    bool 		_isOpen;
};


#endif // YTreeItem_h
