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

  File:		YTableHeader.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <vector>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YTableHeader.h"
#include "YUIException.h"

using std::string;
using std::vector;


struct YTableHeaderPrivate
{
    YTableHeaderPrivate()
	{}

    vector<string>	   headers;
    vector<YAlignmentType> alignments;
};




YTableHeader::YTableHeader()
    : priv( new YTableHeaderPrivate )
{
    YUI_CHECK_NEW( priv );
}


YTableHeader::~YTableHeader()
{
    // NOP
}


void
YTableHeader::addColumn( const string & header, YAlignmentType alignment )
{
    priv->headers.push_back( header );
    priv->alignments.push_back( alignment );
}


int
YTableHeader::columns() const
{
    return (int) priv->headers.size();
}


bool
YTableHeader::hasColumn( int column ) const
{
    return column >= 0 && column < (int) priv->headers.size();
}


string
YTableHeader::header( int column ) const
{
    if ( column >= 0 && column < (int) priv->headers.size() )
	return priv->headers[ column ];
    else
	return "";
}


YAlignmentType
YTableHeader::alignment( int column ) const
{
    if ( column >= 0 && column < (int) priv->alignments.size() )
	return priv->alignments[ column ];
    else
	return YAlignBegin;
}
