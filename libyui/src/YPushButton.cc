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

  File:		YPushButton.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUI.h"
#include "YApplication.h"
#include "YDialog.h"
#include "YUISymbols.h"
#include "YPushButton.h"

using std::string;


struct YPushButtonPrivate
{
    YPushButtonPrivate( const string & label )
	: label( label )
	, isDefaultButton( false )
	, setDefaultButtonRecursive( false )
	, isHelpButton( false )
	, isRelNotesButton( false )
	, role( YCustomButton )
	{}

    string	label;
    bool	isDefaultButton;
    bool	setDefaultButtonRecursive;
    bool	isHelpButton;
    bool	isRelNotesButton;
    YButtonRole	role;
};


YPushButton::YPushButton( YWidget * parent, const string & label )
    : YWidget( parent )
    , priv( new YPushButtonPrivate( label ) )
{
    int fkey = YUI::app()->defaultFunctionKey( label );

    if ( fkey > 0 && ! hasFunctionKey() )
	setFunctionKey( fkey );
}


YPushButton::~YPushButton()
{
    YDialog * dialog = findDialog();

    if ( dialog && dialog->defaultButton() == this )
    {
	dialog->setDefaultButton( 0 );
    }
}


void YPushButton::setLabel( const string & label )
{
    priv->label = label;
}


string YPushButton::label() const
{
    return priv->label;
}


bool YPushButton::isDefaultButton() const
{
    return priv->isDefaultButton;
}


void YPushButton::setDefaultButton( bool isDefaultButton )
{
    priv->isDefaultButton = isDefaultButton;

    if ( ! priv->setDefaultButtonRecursive )
    {
	// Prevent endless recursion if dialog->setDefaultButton()
	// calls this function again

	priv->setDefaultButtonRecursive = true;

	YDialog * dialog = findDialog();

	if ( dialog )
	{
	    if ( isDefaultButton )
		dialog->setDefaultButton( this );
	    else
	    {
		if ( dialog->defaultButton() == this )
		    dialog->setDefaultButton( 0 );
	    }
	}

	priv->setDefaultButtonRecursive = false;
    }
}


bool YPushButton::isHelpButton() const
{
    return priv->isHelpButton;
}


void YPushButton::setHelpButton( bool helpButton )
{
    priv->isHelpButton = helpButton;
    priv->role = YHelpButton;
}

bool YPushButton::isRelNotesButton() const
{
    return priv->isRelNotesButton;
}


void YPushButton::setRelNotesButton( bool relNotesButton )
{
    priv->isRelNotesButton = relNotesButton;
    priv->role = YRelNotesButton;
}

/* setRole can try to guess function key, but only if there isn't a selected
   function key already
*/
void YPushButton::setRole( YButtonRole role )
{
	priv->role = role;
	int old_function_key = functionKey();
	if (!hasFunctionKey()) // otherwise function key was already determined
	{
		switch (priv->role)
		{
			case YOKButton:     YWidget::setFunctionKey( 10 );  break;
			case YCancelButton: YWidget::setFunctionKey( 9 );   break;
			case YApplyButton:  YWidget::setFunctionKey( 10 );  break;
			case YHelpButton:   YWidget::setFunctionKey( 1 );   break;
			default: break;
		}
		if ( functionKey() != old_function_key )
		{
			yuiMilestone() << "Guessing function key F" << functionKey()
			       << " for " << this
			       << " from button role " << priv->role
			       << endl;
		}
	}
}

YButtonRole YPushButton::role() const
{
    return priv->role;
}

/* setFunctionKey (besides setting the function key) should try to guess button
   role, but only if button role is not yet determined.
*/
void YPushButton::setFunctionKey( int fkey_no )
{
    YWidget::setFunctionKey( fkey_no );
    YButtonRole oldRole = priv->role;

	if (priv->role == YCustomButton) // otherwise role was already determined
	{
		switch ( functionKey() )	// base class method might have changed it
		{
			case 10:    priv->role = YOKButton;     break;
			case 9:     priv->role = YCancelButton; break;
			case 1:     priv->role = YHelpButton;   break;
			default:    break;
		}
		if ( priv->role != oldRole )
		{
			yuiMilestone() << "Guessing button role " << priv->role
			       << " for " << this
			       << " from function key F" << functionKey()
			       << endl;
		}
	}
}


const YPropertySet &
YPushButton::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property string     Label	text on the button
	 */
	propSet.add( YProperty( YUIProperty_Label, YStringProperty ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YPushButton::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if ( propertyName == YUIProperty_Label )	setLabel( val.stringVal() );
    else
    {
	YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special handling necessary
}


YPropertyValue
YPushButton::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if ( propertyName == YUIProperty_Label )	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}


std::ostream & operator<<( std::ostream & stream, YButtonRole role )
{
    switch ( role )
    {
	case YCustomButton:	stream << "YCustomButton";	break;
	case YOKButton:		stream << "YOKButton";		break;
	case YApplyButton:	stream << "YApplyButton"; 	break;
	case YCancelButton:	stream << "YCancelButton";	break;
	case YHelpButton:	stream << "YHelpButton";	break;
	case YRelNotesButton:	stream << "YRelNotesButton";	break;

	default:
	    stream << "<Undefined button role #" << (int) role << ">";
	    break;
    }

    return stream;
}
