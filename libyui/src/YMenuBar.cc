/*
  Copyright (c) [2020] SUSE LLC
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

  File:		YMenuBar.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YShortcut.h"
#include "YMenuBar.h"


using std::string;


struct YMenuBarPrivate
{
    YMenuBarPrivate()
	{}

    int dummy;
};




YMenuBar::YMenuBar( YWidget * parent )
    : YMenuWidget( parent,
                   "" )         // label
    , priv( new YMenuBarPrivate() )
{
    YUI_CHECK_NEW( priv );

    setStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT,  false );
}


YMenuBar::~YMenuBar()
{
    // NOP
}


YMenuItem *
YMenuBar::addMenu( const std::string & label,
                   const std::string & iconName )
{
    YMenuItem * menu = new YMenuItem( label, iconName );
    addItem( menu );

    return menu;
}


const YPropertySet &
YMenuBar::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemList	Items		All menu items and submenus
	 * @property itemList	EnabledItems    Enabled or disabled status for items
	 * @property string	IconPath	Base path for icons (on menu items)
	 */
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_EnabledItems,       YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YMenuBar::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_EnabledItems 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_IconPath 	)	setIconBasePath( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YMenuBar::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_EnabledItems 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_IconPath	)	return YPropertyValue( iconBasePath() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}

