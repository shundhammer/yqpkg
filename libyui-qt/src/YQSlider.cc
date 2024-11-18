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

  File:	      YQSlider.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQSlider.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"

using std::string;


YQSlider::YQSlider( YWidget *		parent,
		    const string &	label,
		    int 		minValue,
		    int 		maxValue,
		    int 		initialValue,
		    bool		reverseLayout )

    : QFrame( (QWidget *) parent->widgetRep() )
    , YSlider( parent, label, minValue, maxValue )
{
    setWidgetRep( this );

    QVBoxLayout* toplayout = new QVBoxLayout( this );
    setLayout( toplayout );

    toplayout->setSpacing( YQWidgetSpacing );
    toplayout->setMargin ( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    toplayout->addWidget( _caption );

    _hbox = new QFrame( this );
    YUI_CHECK_NEW( _hbox );
    toplayout->addWidget( _hbox );

    QHBoxLayout *layout = new QHBoxLayout( _hbox );
    _hbox->setLayout( layout );

    layout->setMargin ( YQWidgetMargin );
    layout->setSpacing( YQWidgetSpacing );

    if ( reverseLayout )
    {
        _qt_spinBox = new QSpinBox( _hbox );
        _qt_spinBox->setMinimum(minValue);
        _qt_spinBox->setMaximum(maxValue);
        _qt_spinBox->setSingleStep(1);
	layout->addWidget( _qt_spinBox );
    }
    else
    {
	_caption->setAlignment( Qt::AlignRight );
    }

    _qt_slider = new QSlider( Qt::Horizontal, _hbox );
    _qt_slider->setMinimum(minValue);
    _qt_slider->setMaximum(maxValue);
    _qt_slider->setPageStep(1);
    YUI_CHECK_NEW( _qt_slider );
    layout->addWidget( _qt_slider );

    if ( ! reverseLayout )
    {
        _qt_spinBox = new QSpinBox( _hbox );
        _qt_spinBox->setMinimum(minValue);
        _qt_spinBox->setMaximum(maxValue);
        _qt_spinBox->setSingleStep(1);

	layout->addWidget( _qt_spinBox );
    }
    YUI_CHECK_NEW( _qt_spinBox );

    _qt_spinBox->setValue( initialValue );
    _caption->setBuddy( _qt_spinBox );

    setValue( initialValue );

    connect( _qt_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	     _qt_slider,  &pclass(_qt_slider)::setValue );

    connect( _qt_slider,  &pclass(_qt_slider)::valueChanged,
	     _qt_spinBox, &pclass(_qt_spinBox)::setValue );

    connect( _qt_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
	     this,  	  &pclass(this)::valueChangedSlot );
}


YQSlider::~YQSlider()
{
    // NOP
}


int
YQSlider::value()
{
    return _qt_spinBox->value();
}


void
YQSlider::setValueInternal( int newValue )
{
    YQSignalBlocker sigBlocker1( _qt_spinBox );
    YQSignalBlocker sigBlocker2( _qt_slider  );
    _qt_slider->setValue ( newValue );
    _qt_spinBox->setValue( newValue );
}


void
YQSlider::valueChangedSlot( int newValue )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );

    emit valueChanged( newValue );
}


void
YQSlider::setEnabled( bool enabled )
{
    _caption->setEnabled  ( enabled );
    _qt_slider->setEnabled ( enabled );
    _qt_spinBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQSlider::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ? _caption->sizeHint().width() : 0;

    // Arbitrary value - there is no really good default
    return std::max( 200, hintWidth );
}


int
YQSlider::preferredHeight()
{
    return sizeHint().height();
}


void
YQSlider::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void
YQSlider::setLabel( const string & newLabel )
{
    _caption->setText( newLabel );
    YSlider::setLabel( newLabel );
}


bool
YQSlider::setKeyboardFocus()
{
    _qt_spinBox->setFocus();

    return true;
}
