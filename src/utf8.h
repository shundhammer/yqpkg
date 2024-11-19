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

  File:	      utf8.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef utf8_h
#define utf8_h

#include <qstring.h>
#include <string>
#include <iosfwd>


inline QString fromUTF8( const std::string & str )
{
    return QString::fromUtf8( str.c_str() );
}


inline std::string toUTF8( const QString & str )
{
    if ( str.isEmpty() )
	return std::string( "" );
    else
	return std::string( str.toUtf8().data() );
}


inline std::ostream & operator<<( std::ostream & stream, const QString & str )
{
    return stream << str.toUtf8().data();
}


#endif // utf8_h
