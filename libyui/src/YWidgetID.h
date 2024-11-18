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

  File:		YWidgetID.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YWidgetID_h
#define YWidgetID_h

#include <iosfwd>
#include <string>



/**
 * Abstract base class for widget IDs.
 **/
class YWidgetID
{
protected:
    /**
     * Constructor. Protected since this is an abstract base class.
     **/
    YWidgetID() {}

public:
    /**
     * Destructor.
     **/
    virtual ~YWidgetID() {}

    /**
     * Check if this ID is equal to another.
     **/
    virtual bool isEqual( YWidgetID * otherID ) const = 0;

    /**
     * Convert the ID value to string.
     * Used for logging and debugging.
     **/
    virtual std::string toString() const = 0;

private:
    /**
     * Copy constructor is disabled.
     **/
    YWidgetID( const YWidgetID & orig );
};


/**
 * Simple widget ID class based on strings.
 **/
class YStringWidgetID: public YWidgetID
{
public:

    /**
     * Constructor.
     **/
    YStringWidgetID( const std::string & value );

    /**
     * Destructor.
     **/
    virtual ~YStringWidgetID();

    /**
     * Check if this ID is equal to another.
     *
     * Reimplemented from YWidgetID.
     **/
    virtual bool isEqual( YWidgetID * otherID ) const;

    /**
     * Convert the ID value to string.
     * Used for logging and debugging.
     *
     * Reimplemented from YWidgetID.
     **/
    virtual std::string toString() const;

    /**
     * Return the ID value.
     **/
    std::string value() const;

    /**
     * Return the ID value as a const ref.
     **/
    const std::string & valueConstRef() const;

private:
    std::string _value;
};

std::ostream & operator<<( std::ostream & stream, const YWidgetID * id );


#endif // YWidgetID_h
