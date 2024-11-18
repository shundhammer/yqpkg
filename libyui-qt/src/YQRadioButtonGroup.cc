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

  File:	      YQRadioButtonGroup.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QRadioButton>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "YQRadioButtonGroup.h"
#include "YQRadioButton.h"
#include "qstring.h"


YQRadioButtonGroup::YQRadioButtonGroup( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YRadioButtonGroup( parent )
{
    setWidgetRep( this );
}


YQRadioButtonGroup::~YQRadioButtonGroup()
{
    // NOP
}


void
YQRadioButtonGroup::addRadioButton( YRadioButton * button )
{
    YRadioButtonGroup::addRadioButton( button );

    if ( button->value() )		// if this new button is active
    {
	uncheckOtherButtons( button );	// make it the only active one
    }
}


void
YQRadioButtonGroup::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void
YQRadioButtonGroup::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YRadioButtonGroup::setSize( newWidth, newHeight );
}



