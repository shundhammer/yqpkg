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

  File:		YTimezoneSelector.cc

  Author:	Stephan Kulow <coolo@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YTimezoneSelector.h"

using std::string;


class YTimezoneSelectorPrivate
{
   bool dummy;
};




YTimezoneSelector::YTimezoneSelector( YWidget *				  parent,
                                      const string			& pixmap,
                                      const std::map<string, string>	& timezones )
    : YWidget( parent )
{
}


YTimezoneSelector::~YTimezoneSelector()
{
    // NOP
}


const YPropertySet &
YTimezoneSelector::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
        propSet.add( YProperty( YUIProperty_Value, YStringProperty ) );
        propSet.add( YProperty( YUIProperty_CurrentItem, YStringProperty ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YTimezoneSelector::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if ( propertyName == YUIProperty_Value )
    {
        setCurrentZone( val.stringVal(), true );
        return true; // success -- no special handling necessary
    }
    if ( propertyName == YUIProperty_CurrentItem )
    {
        setCurrentZone( val.stringVal(), false );
        return true; // success -- no special handling necessary
    }
    return YWidget::setProperty( propertyName, val );
}


YPropertyValue
YTimezoneSelector::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if ( propertyName == YUIProperty_Value ) 	        return YPropertyValue( currentZone() );
    if ( propertyName == YUIProperty_CurrentItem )      return YPropertyValue( currentZone() );
    
    return YWidget::getProperty( propertyName );
}
