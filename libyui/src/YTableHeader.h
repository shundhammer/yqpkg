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

  File:		YTableHeader.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YTableHeader_h
#define YTableHeader_h

#include <string>
#include "ImplPtr.h"
#include "YTypes.h"



class YTableHeaderPrivate;

/**
 * Helper class for YTable for table column properties:
 *
 *   - number of columns
 *   - header    for each column
 *   - alignment for each column
 **/
class YTableHeader
{
public:
    /**
     * Constructor.
     **/
    YTableHeader();

    /**
     * Destructor.
     **/
    virtual ~YTableHeader();

    /**
     * Add a column with the specified colum header text and alignment.
     **/
    void addColumn( const std::string &	header,
		    YAlignmentType	alignment = YAlignBegin );

    /**
     * Return the number of columns.
     **/
    int columns() const;

    /**
     * Return 'true' if this table header has a column no. 'column'
     * (counting from 0 on).
     **/
    bool hasColumn( int column ) const;

    /**
     * Return the header text for the specified column.
     **/
    std::string header( int column ) const;

    /**
     * Return the alignment for the specified column.
     **/
    YAlignmentType alignment( int column ) const;


private:

    ImplPtr<YTableHeaderPrivate> priv;
};


#endif // YTableHeader_h
