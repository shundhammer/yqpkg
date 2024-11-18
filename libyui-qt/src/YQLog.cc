/*
  Copyright (c) 2021 SUSE LLC
  
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


#include <iostream>
#include "YQLog.h"

#include <QPoint>
#include <QSize>
#include <QRect>


std::ostream & operator<<( std::ostream & stream, const QPoint & pos )
{
    stream << "QPoint( x: " << pos.x() << "; y: " << pos.y() << " )";
    
    return stream;
}


std::ostream & operator<<( std::ostream & stream, const QSize & size )
{
    stream << "QSize( w: " << size.width() << "; h: " << size.height() << " )";
    
    return stream;
}


std::ostream & operator<<( std::ostream & stream, const QRect & rect )
{
    stream << "QRect( x: " << rect.x()
           << "; y: "      << rect.y()
           << "; w: "      << rect.width()
           << "; h: "      << rect.height()
           << " )";
    
    return stream;
}


