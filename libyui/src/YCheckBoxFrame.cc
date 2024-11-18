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

  File:		YCheckBoxFrame.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YCheckBoxFrame.h"

using std::string;


struct YCheckBoxFramePrivate
{
    YCheckBoxFramePrivate( const string & label )
	: label( label )
	, autoEnable( true )
	, invertAutoEnable( false )
	{}

    string	label;
    bool	autoEnable;
    bool	invertAutoEnable;
};




YCheckBoxFrame::YCheckBoxFrame( YWidget *	parent,
				const string &	label,
				bool		isChecked )
    : YSingleChildContainerWidget( parent )
    , priv( new YCheckBoxFramePrivate( label ) )
{
    YUI_CHECK_NEW( priv );
}


YCheckBoxFrame::~YCheckBoxFrame()
{
    // NOP
}


string YCheckBoxFrame::label() const
{
    return priv->label;
}


void YCheckBoxFrame::setLabel( const string & label )
{
    priv->label = label;
}


bool YCheckBoxFrame::autoEnable() const
{
    return priv->autoEnable;
}


void YCheckBoxFrame::setAutoEnable( bool autoEnable )
{
    // yuiDebug() << "Auto enable: " << boolalpha << autoEnable << endl;
    priv->autoEnable = autoEnable;
}


bool YCheckBoxFrame::invertAutoEnable() const
{
    return priv->invertAutoEnable;
}


void YCheckBoxFrame::setInvertAutoEnable( bool invertAutoEnable )
{
    // yuiDebug() << "Invert auto enable: ", boolalpha << invertAutoEnable << endl;
    priv->invertAutoEnable = invertAutoEnable;
}


void YCheckBoxFrame::handleChildrenEnablement( bool enabled )
{
    if ( autoEnable() )
    {
	if ( invertAutoEnable() )
	    enabled = ! enabled;

	yuiDebug() << ( enabled ? "Enabling" : "Disabling" ) << " child widgets of " << this << endl;
	setChildrenEnabled( enabled );
    }
}


const YPropertySet &
YCheckBoxFrame::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property boolean    Value 	the on/off state of the CheckBoxFrame's check box
	 * @property string     Label	the text on the frame
	 */

	propSet.add( YProperty( YUIProperty_Value,	YBoolProperty	) );
	propSet.add( YProperty( YUIProperty_Label,	YStringProperty	) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YCheckBoxFrame::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Value )	setValue( val.boolVal() );
    else if ( propertyName == YUIProperty_Label )	setLabel( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YCheckBoxFrame::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Value )	return YPropertyValue( value() );
    else if ( propertyName == YUIProperty_Label )	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
