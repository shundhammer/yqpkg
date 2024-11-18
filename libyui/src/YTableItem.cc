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

  File:		YTableItem.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define MIN_DEBUG_LABEL_LEN	20
#define MAX_DEBUG_LABEL_LEN	40


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YTableItem.h"
#include "YUIException.h"

using std::string;


YTableItem::YTableItem()
    : YTreeItem( "" )
{
    // NOP
}


YTableItem::YTableItem( YTableItem * parent,
                        bool         isOpen )
    : YTreeItem( parent, "", isOpen )
{
    // NOP
}


YTableItem::YTableItem( const string & label_0,
			const string & label_1,
			const string & label_2,
			const string & label_3,
			const string & label_4,
			const string & label_5,
			const string & label_6,
			const string & label_7,
			const string & label_8,
			const string & label_9 )
    : YTreeItem( "" )
{
    addCells( label_0,
              label_1,
              label_2,
              label_3,
              label_4,
              label_5,
              label_6,
              label_7,
              label_8,
              label_9 );
}


YTableItem::YTableItem( YTableItem *   parent,
                        const string & label_0,
			const string & label_1,
			const string & label_2,
			const string & label_3,
			const string & label_4,
			const string & label_5,
			const string & label_6,
			const string & label_7,
			const string & label_8,
			const string & label_9 )
    : YTreeItem( parent, "" )
{
    addCells( label_0,
              label_1,
              label_2,
              label_3,
              label_4,
              label_5,
              label_6,
              label_7,
              label_8,
              label_9 );
}


YTableItem::~YTableItem()
{
    deleteCells();
}


void
YTableItem::deleteCells()
{
    YTableCellIterator it = cellsBegin();

    while ( it != cellsEnd() )
    {
	YTableCell * cell = *it;
	++it;
	delete cell;
    }

    _cells.clear();
}


void
YTableItem::addCell( YTableCell * cell )
{
    YUI_CHECK_PTR( cell );
    _cells.push_back( cell );

    cell->reparent( this, _cells.size() - 1 );
}


void
YTableItem::addCell( const string & label,
                     const string & iconName,
                     const string & sortKey )
{
    YTableCell * cell = new YTableCell( label, iconName, sortKey );
    YUI_CHECK_NEW( cell );

    addCell( cell );
}


void
YTableItem::addCells( const std::string & label_0,
                      const std::string & label_1,
                      const std::string & label_2,
                      const std::string & label_3,
                      const std::string & label_4,
                      const std::string & label_5,
                      const std::string & label_6,
                      const std::string & label_7,
                      const std::string & label_8,
                      const std::string & label_9 )
{
    std::vector<string> labels;
    labels.reserve(10); // slight optimization
    labels.push_back( label_0 );
    labels.push_back( label_1 );
    labels.push_back( label_2 );
    labels.push_back( label_3 );
    labels.push_back( label_4 );
    labels.push_back( label_5 );
    labels.push_back( label_6 );
    labels.push_back( label_7 );
    labels.push_back( label_8 );
    labels.push_back( label_9 );

    //
    // Find the last non-empty label
    //

    unsigned lastLabel = labels.size() - 1;

    while ( labels[ lastLabel ].empty() && --lastLabel > 0 )
    {}

    //
    // Create cells
    //

    for ( unsigned i = 0; i <= lastLabel; ++i )
    {
	addCell( labels[i] );
    }
}


bool
YTableItem::hasCell( int index ) const
{
    return index >= 0 && (unsigned) index < _cells.size();
}


const YTableCell *
YTableItem::cell( int index ) const
{
    return hasCell( index ) ?
	_cells[ index ] : 0;
}


YTableCell *
YTableItem::cell( int index )
{
    return hasCell( index ) ?
	_cells[ index ] : 0;
}


string
YTableItem::label( int index ) const
{
    return hasCell( index ) ? _cells[ index ]->label() : "";
}


string
YTableItem::iconName( int index ) const
{
    return hasCell( index ) ? _cells[ index ]->iconName() : "";
}


bool
YTableItem::hasIconName( int index ) const
{
    return hasCell( index ) ? _cells[ index ]->hasIconName() : false;
}


string
YTableItem::debugLabel() const
{
    if ( _cells.empty() )
        return "[empty]";

    string txt;

    for ( unsigned i=0; i < _cells.size(); ++i )
    {
        if ( ! txt.empty() )
            txt += " | ";

        txt += label( i );

        if ( txt.size() > MIN_DEBUG_LABEL_LEN )
            break;
    }

    return limitLength( txt, MAX_DEBUG_LABEL_LEN );
}


//----------------------------------------------------------------------


void YTableCell::reparent( YTableItem * parent, int column )
{
    YUI_CHECK_PTR( parent );

    if ( _parent && _parent != parent && _column != column )
	YUI_THROW( YUIException( string( "Cannot reparent YTableCell \"" )
				 + _label
				 + "to different parent." ) );
    _parent = parent;
    _column = column;
}
