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

  File:		YMultiSelectionBox.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YMultiSelectionBox.h"
#include "YUISymbols.h"
#include "YMacroRecorder.h"
#include "YUIException.h"

using std::string;


struct YMultiSelectionBoxPrivate
{
    YMultiSelectionBoxPrivate()
	: shrinkable( false )
	{}

    bool shrinkable;
};



YMultiSelectionBox::YMultiSelectionBox( YWidget * parent, const string & label )
    : YSelectionWidget( parent, label,
			false )		// enforceSingleSelection
    , priv( new YMultiSelectionBoxPrivate )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );

}


YMultiSelectionBox::~YMultiSelectionBox()
{
    // NOP
}


bool YMultiSelectionBox::shrinkable() const
{
    return priv->shrinkable;
}


void YMultiSelectionBox::setShrinkable( bool shrinkable )
{
    priv->shrinkable = shrinkable;
}


const YPropertySet &
YMultiSelectionBox::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property itemList	SelectedItems	All currently selected items
	 * @property itemList	Items		All items
	 * @property itemID	CurrentItem	The current item (no matter if selected or not)
	 * @property string	Label		Caption above the MultiSelectionBox
	 */
	propSet.add( YProperty( YUIProperty_CurrentItem,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_SelectedItems,	YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Items,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Label,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_IconPath,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YMultiSelectionBox::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_CurrentItem 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_SelectedItems	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Items 	)	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Label		)	setLabel( val.stringVal() );
    else if ( propertyName == YUIProperty_IconPath      )       setIconBasePath( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YMultiSelectionBox::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_CurrentItem 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_SelectedItems	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Items 	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Label		)	return YPropertyValue( label() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}



void
YMultiSelectionBox::saveUserInput( YMacroRecorder *macroRecorder )
{
    macroRecorder->recordWidgetProperty( this, YUIProperty_CurrentItem   );
    macroRecorder->recordWidgetProperty( this, YUIProperty_SelectedItems );
}
