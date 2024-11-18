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

  File:		YDateField.cc

  Author:     	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YDateField.h"

using std::string;


struct YDateFieldPrivate
{
    YDateFieldPrivate()
	{}

    bool dummy;
};




YDateField::YDateField( YWidget * parent, const string & label )
    : YSimpleInputField( parent, label )
    , priv( new YDateFieldPrivate() )
{
    YUI_CHECK_NEW( priv );
}


YDateField::~YDateField()
{
    // NOP
}


/*
 * Properties (all handled in YSimpleInputField):
 *
 * @property string	Value	the date (the field's contents) as "YYYY-MM-DD"
 * @property string	Label	caption above the input field
 */
