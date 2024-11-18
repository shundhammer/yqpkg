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

  File:		YTimeField.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YTimeField_h
#define YTimeField_h

#include "YSimpleInputField.h"

class YTimeFieldPrivate;


/**
 * Input field for entering a time in "hh:mm:ss" format.
 *
 * Derived classes are required to implement:
 *     	value()
 *	setValue()
 * See YSimpleInputField.h for details.
 **/
class YTimeField : public YSimpleInputField
{
protected:

    /**
     * Constructor.
     **/
    YTimeField( YWidget * parent, const std::string & label );

public:

    /**
     * Destructor.
     **/
    virtual ~YTimeField();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YTimeField"; }

private:

    ImplPtr<YTimeFieldPrivate> priv;
};


#endif // YTimeField_h
