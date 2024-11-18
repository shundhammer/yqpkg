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

  File:		YBothDim.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YBothDim_h
#define YBothDim_h

#include "YTypes.h"
#include "YUIException.h"


/**
 * Template class for two-dimensional entities, such as
 * - width, height
 * - x_pos, y_pos
 * - hStretchable, vStretchable
 *
 * Precondition: type T needs to have a default constructor
 * (which all simple types like int, long, bool have).
 **/
template<typename T> class YBothDim
{
public:

    // Data Members - intentionally public
    T vert;
    T hor;

    /**
     * Constructor with explicit initialization for both values
     **/
    YBothDim( T hor, T vert )
	: vert( vert )
	, hor( hor )
	{}

    /**
     * Default constructor (calls T default constructor for both values)
     **/
    YBothDim()
	{}

    /**
     * operator[] for alternative access via  myVar[ YD_HORIZ ]
     * Please note that this returns a non-const reference, so this can be used
     * as an lvalue (e.g., in assignments)
     **/
    T & operator[]( YUIDimension dim )
    {
	switch ( dim )
	{
	    case YD_HORIZ:	return hor;
	    case YD_VERT:	return vert;
	    default:		YUI_THROW( YUIInvalidDimensionException() );
	}

	// never reached (but gcc will complain otherwise)
	return hor;
    }

    /**
     * Same as above for const objects
     **/
    const T & operator[]( YUIDimension dim ) const
    {
	switch ( dim )
	{
	    case YD_HORIZ:	return hor;
	    case YD_VERT:	return vert;
	    default:		YUI_THROW( YUIInvalidDimensionException() );
	}

	// never reached (but gcc will complain otherwise)
	return hor;
    }
};


#endif // YBothDim_h
