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

  File:		YTimeField.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YTimeField.h"

using std::string;


struct YTimeFieldPrivate
{
    YTimeFieldPrivate()
	{}

    bool dummy;
};




YTimeField::YTimeField( YWidget * parent, const string & label )
    : YSimpleInputField( parent, label )
    , priv( new YTimeFieldPrivate() )
{
    YUI_CHECK_NEW( priv );
}


YTimeField::~YTimeField()
{
    // NOP
}


/*
 * Properties (all handled in YSimpleInputField):
 *
 * @property string	Value	the time (the field's contents) as "hh:mm:ss"
 * @property string	Label	caption above the input field
 */
