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

  File:		YRadioButton.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <string>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YUIException.h"
#include "YMacroRecorder.h"
#include "YRadioButtonGroup.h"
#include "YRadioButton.h"

using std::string;


struct YRadioButtonPrivate
{
    /**
     * Constructor
     **/
    YRadioButtonPrivate( const string & label )
	: label( label )
	, radioButtonGroup( 0 )
	, useBoldFont( false )
	{}

    //
    // Data members
    //

    string		label;
    YRadioButtonGroup * radioButtonGroup;
    bool		useBoldFont;
};


YRadioButton::YRadioButton( YWidget * 		parent,
			    const string &	label )
    : YWidget( parent )
    , priv( new YRadioButtonPrivate( label ) )
{
    YUI_CHECK_NEW( priv );

    // Intentionally not calling
    //     buttonGroup()->addRadioButton( this );
    // here because virtual functions can't be used yet (while the constructor
    // isn't finished yet), and the RadioButtonGroup for sure would try to call
    // YRadioButton::value() which is (pure) virtual, thus not available yet.
    //
    // The caller has to take care of this.
}


YRadioButton::~YRadioButton()
{
    if ( priv->radioButtonGroup )
    {
	if ( ! priv->radioButtonGroup->beingDestroyed() )
	    priv->radioButtonGroup->removeRadioButton( this );
    }
}


void YRadioButton::setLabel( const string & newLabel )
{
    priv->label = newLabel;
}


string YRadioButton::label() const
{
    return priv->label;
}


bool YRadioButton::useBoldFont() const
{
    return priv->useBoldFont;
}


void YRadioButton::setUseBoldFont( bool bold )
{
    priv->useBoldFont = bold;
}


const YPropertySet &
YRadioButton::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property boolean	Value	the on/off state of the RadioButton
	 * @property string	Label	the text on the RadioButton
	 */

	propSet.add( YProperty( YUIProperty_Value,	YBoolProperty	) );
	propSet.add( YProperty( YUIProperty_Label,	YStringProperty	) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YRadioButton::setProperty( const string & propertyName, const YPropertyValue & val )
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
YRadioButton::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Value )	return YPropertyValue( value() );
    else if ( propertyName == YUIProperty_Label )	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}


YRadioButtonGroup *
YRadioButton::buttonGroup()
{
    if ( ! priv->radioButtonGroup )
    {
	priv->radioButtonGroup = findRadioButtonGroup();
    }

    return priv->radioButtonGroup;
}


YRadioButtonGroup *
YRadioButton::findRadioButtonGroup() const
{
    YWidget * widget = parent();

    while ( widget )
    {
	YRadioButtonGroup * radioButtonGroup = dynamic_cast<YRadioButtonGroup *> (widget);

	if ( radioButtonGroup )
	    return radioButtonGroup;
	else
	    widget = widget->parent();
    }

    return 0;
}


void
YRadioButton::saveUserInput( YMacroRecorder *macroRecorder )
{
    if ( value() )
    {
	// Only record if this radio button is on. By definition one radio
	// button of the radio box _must_ be on if the user did anything, so we
	// don't record a lot of redundant "ChangeWidget( ..., :Value, false )"
	// calls.

	macroRecorder->recordWidgetProperty( this, YUIProperty_Value );
    }
}
