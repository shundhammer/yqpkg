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

  File:		YDateField.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YDateField_h
#define YDateField_h

#include "YSimpleInputField.h"

class YDateFieldPrivate;

/**
 * Input field for entering a date.
 *
 * Derived classes are required to implement:
 *     	value()
 *	setValue()
 *
 * For both methods the date is formatted as "YYYY-MM-DD".
 * See YSimpleInputField.h for more details.
 **/
class YDateField : public YSimpleInputField
{
protected:
    /**
     * Constructor.
     **/
    YDateField( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YDateField();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YDateField"; }


private:

    ImplPtr<YDateFieldPrivate> priv;
};


#endif // YDateField_h
