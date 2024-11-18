/*
  Copyright (c) [2020] SUSE LLC
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

  File:		YMenuItem.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include "YMenuItem.h"


YMenuItem * YMenuItem::addItem( const std::string & label,
                                const std::string & iconName )
{
    return new YMenuItem( this, label, iconName );
}


YMenuItem * YMenuItem::addMenu( const std::string & label,
                                const std::string & iconName )
{
    return new YMenuItem( this, label, iconName );
}


YMenuItem * YMenuItem::addSeparator()
{
    return new YMenuItem( this, "" );
}
