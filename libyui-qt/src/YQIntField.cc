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

  File:	      YQIntField.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qspinbox.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQIntField.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>

using std::string;



YQIntField::YQIntField( YWidget *	parent,
			const string &	label,
			int 		minValue,
			int 		maxValue,
			int 		initialValue )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YIntField( parent, label, minValue, maxValue )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption    = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_spinBox = new QSpinBox(this);
    _qt_spinBox->setMinimum(minValue);
    _qt_spinBox->setMaximum(maxValue);
    _qt_spinBox->setSingleStep(1);

    YUI_CHECK_NEW( _qt_spinBox );
    layout->addWidget( _qt_spinBox );

    _qt_spinBox->setValue( initialValue );

    _caption->setBuddy( _qt_spinBox );

    setValue( initialValue );

    connect( _qt_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	     this,  	  &pclass(this)::valueChangedSlot );
}


YQIntField::~YQIntField()
{
    // NOP
}


int
YQIntField::value()
{
    return _qt_spinBox->value();
}


void
YQIntField::setValueInternal( int newValue )
{
    YQSignalBlocker sigBlocker( _qt_spinBox );
    _qt_spinBox->setValue( newValue );
}


void
YQIntField::valueChangedSlot( int newValue )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
    emit valueChanged( newValue );
}


void
YQIntField::setLabel( const string & newLabel )
{
    YIntField::setLabel( newLabel );
    _caption->setText( newLabel );
}


void
YQIntField::setEnabled( bool enabled )
{
    _caption->setEnabled  ( enabled );
    _qt_spinBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQIntField::preferredWidth()
{
    return sizeHint().width();
}


int
YQIntField::preferredHeight()
{
    return sizeHint().height();
}


void
YQIntField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQIntField::setKeyboardFocus()
{
    _qt_spinBox->setFocus();

    return true;
}
