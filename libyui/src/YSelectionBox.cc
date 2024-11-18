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

  File:		YSelectionBox.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YSelectionBox.h"
#include "YUISymbols.h"
#include "YUIException.h"

using std::string;


struct YSelectionBoxPrivate
{
    YSelectionBoxPrivate()
	: shrinkable( false )
	, immediateMode( false )
	{}

    bool shrinkable;
    bool immediateMode;
};




YSelectionBox::YSelectionBox( YWidget * parent, const string & label )
    : YSelectionWidget( parent, label,
			true ) // enforceSingleSelection
    , priv( new YSelectionBoxPrivate() )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YSelectionBox::~YSelectionBox()
{
    // NOP
}


bool YSelectionBox::shrinkable() const
{
    return priv->shrinkable;
}


void YSelectionBox::setShrinkable( bool shrinkable )
{
    priv->shrinkable = shrinkable;
}


bool YSelectionBox::immediateMode() const
{
    return priv->immediateMode;
}


void YSelectionBox::setImmediateMode( bool immediateMode )
{
    priv->immediateMode = immediateMode;

    if ( immediateMode )
	setNotify( true );
}


const YPropertySet &
YSelectionBox::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemID	Value		The currently selected item
	 * @property itemID	CurrentItem	The currently selected item
	 * @property itemList	Items		All items
	 * @property string	Label		Caption above the selection box
	 * @property string	IconPath	Base path for icons
	 */
	propSet.add( YProperty( YUIProperty_Value,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YSelectionBox::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_CurrentItem 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else if ( propertyName == YUIProperty_IconPath 	)	setIconBasePath( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YSelectionBox::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_CurrentItem 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else if ( propertyName == YUIProperty_IconPath	)	return YPropertyValue( iconBasePath() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
