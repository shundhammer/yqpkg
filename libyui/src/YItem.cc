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

  File:		YItem.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define MAX_DEBUG_LABEL_LEN	32


#include <iostream>
#include "YItem.h"

using std::string;


/**
 * Static children collection that is always empty so the children
 * iterators of this base class have something valid to return.
 *
 * No item will ever be added to this collection.
 **/
YItemCollection YItem::_noChildren;


string
YItem::debugLabel() const
{
    return limitLength( _label, MAX_DEBUG_LABEL_LEN );
}


string
YItem::limitLength( const string & origText, int limit ) const
{
    string text = origText;

    if ( text.size() > MAX_DEBUG_LABEL_LEN )
    {
	text.resize( MAX_DEBUG_LABEL_LEN );
	text.append( "..." );
    }

    return text;
}


std::ostream & operator<<( std::ostream & stream, const YItem * item )
{
    if ( item )
        stream << "<" << item->itemClass() << " " << item->debugLabel() << ">";
    else
        stream << "<NULL YItem>";

    return stream;
}
