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

  File:		YCheckBox.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YCheckBox.h"

using std::string;


struct YCheckBoxPrivate
{
    YCheckBoxPrivate( const string & label )
	: label( label )
	, useBoldFont( false )
	{}

    string	label;
    bool	useBoldFont;
};


YCheckBox::YCheckBox( YWidget * parent, const string & label )
    : YWidget( parent )
    , priv( new YCheckBoxPrivate( label ) )
{
    YUI_CHECK_NEW( priv );
}


YCheckBox::~YCheckBox()
{
    // NOP
}


void YCheckBox::setLabel( const string & newLabel )
{
    priv->label = newLabel;
}


string YCheckBox::label() const
{
    return priv->label;
}


bool YCheckBox::useBoldFont() const
{
    return priv->useBoldFont;
}


void YCheckBox::setUseBoldFont( bool bold )
{
    priv->useBoldFont = bold;
}


const YPropertySet &
YCheckBox::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property boolean	Value 	the on/off state; nil for "don't care" (tristate)
	 * @property string     Label	the text on the CheckBox
	 */

	propSet.add( YProperty( YUIProperty_Value,	YOtherProperty	) );
	propSet.add( YProperty( YUIProperty_Label,	YStringProperty	) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YCheckBox::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Value )	return false; // need special processing
    else if ( propertyName == YUIProperty_Label )	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YCheckBox::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Value )	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Label )	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
