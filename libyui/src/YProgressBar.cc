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

  File:		YProgressBar.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YProgressBar.h"

using std::string;


struct YProgressBarPrivate
{
    YProgressBarPrivate( const string &	label,
			 int		maxValue )
	: label( label )
	, maxValue( maxValue )
	, value( 0 )
	{
	    if ( maxValue < 1 )
		maxValue = 1;
	}

    string	label;
    int 	maxValue;
    int		value;
};




YProgressBar::YProgressBar( YWidget * 		parent,
			    const string &	label,
			    int			maxValue )
    : YWidget( parent )
    , priv( new YProgressBarPrivate( label, maxValue ) )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT, false );
}


YProgressBar::~YProgressBar()
{
    // NOP
}


string YProgressBar::label()
{
    return priv->label;
}


void YProgressBar::setLabel( const string & label )
{
    priv->label = label;
}


int YProgressBar::maxValue() const
{
    return priv->maxValue;
}


int YProgressBar::value() const
{
    return priv->value;
}


void YProgressBar::setValue( int newValue )
{
    if ( newValue < 0 )
	newValue = 0;

    if ( newValue > priv->maxValue )
	newValue = priv->maxValue;

    priv->value = newValue;
}


const YPropertySet &
YProgressBar::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property integer	Value	the current progress
	 * @property string	Label	caption above the progress bar
	 */
	propSet.add( YProperty( YUIProperty_Value,		YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YProgressBar::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	setValue( val.integerVal() );
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YProgressBar::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( value() );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
