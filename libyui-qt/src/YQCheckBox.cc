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

  File:	      YQCheckBox.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qcheckbox.h>
#include <QBoxLayout>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQApplication.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQCheckBox.h"

#define SPACING 8

using std::string;


YQCheckBox::YQCheckBox( YWidget *	parent,
			const string & 	label,
			bool 		checked )
    : QCheckBox( fromUTF8( label ), (QWidget *) parent->widgetRep() )
    , YCheckBox( parent, label )
{
    setWidgetRep( this );

    QCheckBox::setChecked( checked );

    connect( this, &QCheckBox::stateChanged,
             this, &YQCheckBox::stateChanged );
}


YQCheckBox::~YQCheckBox()
{
    // NOP
}


YCheckBoxState
YQCheckBox::value()
{
    switch ( checkState() )
    {
        case Qt::Checked:          return YCheckBox_on;
	case Qt::Unchecked:	   return YCheckBox_off;
	case Qt::PartiallyChecked: return YCheckBox_dont_care;
    }

    return YCheckBox_off;
}


void
YQCheckBox::setValue( YCheckBoxState newValue )
{
    switch ( newValue )
    {
	case YCheckBox_on:
            QCheckBox::setChecked( true );
	    setTristate( false );
	    break;

	case YCheckBox_off:
	    QCheckBox::setChecked( false );
	    setTristate( false );
	    break;

	case YCheckBox_dont_care:
	    QCheckBox::setTristate( true );
	    setCheckState(Qt::PartiallyChecked);
	    break;
    }
}


void YQCheckBox::setLabel( const string & label )
{
    setText( fromUTF8( label ) );
    YCheckBox::setLabel( label );
}


void YQCheckBox::setUseBoldFont( bool useBold )
{
    setFont( useBold ?
			   YQUI::yqApp()->boldFont() :
			   YQUI::yqApp()->currentFont() );

    YCheckBox::setUseBoldFont( useBold );
}


void YQCheckBox::setEnabled( bool enabled )
{
    QCheckBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQCheckBox::preferredWidth()
{
    return 2*SPACING + sizeHint().width();
}


int YQCheckBox::preferredHeight()
{
    return sizeHint().height();
}


void YQCheckBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQCheckBox::setKeyboardFocus()
{
    setFocus();

    return true;
}


void YQCheckBox::stateChanged( int newState )
{
    // yuiMilestone() << "new state: " << newState << endl;

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}



