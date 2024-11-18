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

  File:		YQTimeField.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTimeField.h"
#include "yui/YEvent.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>

using std::string;


YQTimeField::YQTimeField( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTimeField( parent, label )
{
    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_timeEdit = new QTimeEdit( this );
    YUI_CHECK_NEW( _qt_timeEdit );
    _qt_timeEdit->setDisplayFormat( "hh:mm:ss");
    layout->addWidget( _qt_timeEdit );

    _caption->setBuddy( _qt_timeEdit );

    connect( _qt_timeEdit, &QTimeEdit::timeChanged,
             this,         &YQTimeField::changed);
}


YQTimeField::~YQTimeField()
{
    // NOP
}


string YQTimeField::value()
{
    return toUTF8( _qt_timeEdit->time().toString( Qt::ISODate ) );
}


void YQTimeField::setValue( const string & newValue )
{
    _qt_timeEdit->blockSignals(true);
    _qt_timeEdit->setTime(  QTime::fromString( fromUTF8( newValue ), Qt::ISODate ) );
    _qt_timeEdit->blockSignals(false);
}


void YQTimeField::setLabel( const string & newLabel )
{
    _caption->setText( fromUTF8( newLabel ) );
    YTimeField::setLabel( newLabel );
}


void YQTimeField::setEnabled( bool enabled )
{
    QFrame::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQTimeField::preferredWidth()
{
    return sizeHint().width();
}


int YQTimeField::preferredHeight()
{
    return sizeHint().height();
}


void YQTimeField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQTimeField::setKeyboardFocus()
{
    _qt_timeEdit->setFocus();

    return true;
}


void YQTimeField::changed ( const QTime& )
{
    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}



