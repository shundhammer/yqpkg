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

  File:		YIntField.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YIntField.h"

using std::string;


struct YIntFieldPrivate
{
    YIntFieldPrivate( const string &	label,
		      int		minValue,
		      int		maxValue )
	: label( label )
	, minValue( minValue )
	, maxValue( maxValue )
	{}

    string	label;
    int		minValue;
    int		maxValue;
};




YIntField::YIntField( YWidget *		parent,
		      const string &	label,
		      int		minValue,
		      int		maxValue )
    : YWidget( parent )
    , priv( new YIntFieldPrivate( label, minValue, maxValue ) )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT, false );
}


YIntField::~YIntField()
{
    // NOP
}


int
YIntField::enforceRange( int val ) const
{
    if ( val < priv->minValue )
	val = priv->minValue;

    if ( val > priv->maxValue )
	val = priv->maxValue;

    return val;
}


int
YIntField::minValue() const
{
    return priv->minValue;
}


void
YIntField::setMinValue( int val )
{
    priv->minValue = val;

    int oldValue = value();
    int newValue = enforceRange ( oldValue );

    if ( oldValue != newValue )
	setValue( newValue );	// This might be expensive
}


int
YIntField::maxValue() const
{
    return priv->maxValue;
}


void
YIntField::setMaxValue( int val )
{
    priv->maxValue = val;

    int oldValue = value();
    int newValue = enforceRange ( oldValue );

    if ( oldValue != newValue )
	setValue( newValue );	// This might be expensive
}


string
YIntField::label() const
{
    return priv->label;
}


void
YIntField::setLabel( const string & label )
{
    priv->label = label;
}



const YPropertySet &
YIntField::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property integer	Value		the field's contents (the user input)
	 * @property integer	MinValue	the minimum value
	 * @property integer	MaxValue	the maximum value
	 * @property string	Label		caption above the field
	 */
	propSet.add( YProperty( YUIProperty_Value,		YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_MinValue,		YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_MaxValue,		YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YIntField::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	setValue   ( val.integerVal() );
    else if ( propertyName == YUIProperty_MinValue	)	setMinValue( val.integerVal() );
    else if ( propertyName == YUIProperty_MaxValue	)	setMaxValue( val.integerVal() );
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YIntField::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( value() );
    if	    ( propertyName == YUIProperty_MinValue	)	return YPropertyValue( minValue() );
    if	    ( propertyName == YUIProperty_MaxValue	)	return YPropertyValue( maxValue() );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
