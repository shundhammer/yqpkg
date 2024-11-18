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

  File:	      	YQDateField.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <qdatetimeedit.h>
#include <QVBoxLayout>

#include "utf8.h"
#include "YQUI.h"
#include "YQDateField.h"
#include "yui/YEvent.h"
#include "YQWidgetCaption.h"

using std::string;


YQDateField::YQDateField( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YDateField( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_dateEdit = new QDateEdit( this );
    YUI_CHECK_NEW( _qt_dateEdit );
    layout->addWidget( _qt_dateEdit );

    //_qt_dateEdit->setAutoAdvance( true );
    _qt_dateEdit->setDisplayFormat( "yyyy-MM-dd" );
    _qt_dateEdit->setCalendarPopup(true);
    _caption->setBuddy( _qt_dateEdit );

    connect( _qt_dateEdit, &QDateEdit::dateChanged,
             this,         &YQDateField::changed);
}


YQDateField::~YQDateField()
{
    // NOP
}


string YQDateField::value()
{
    return toUTF8( _qt_dateEdit->date().toString( Qt::ISODate ) );
}


void YQDateField::setValue( const string & newValue )
{
    _qt_dateEdit->blockSignals(true);
    _qt_dateEdit->setDate( QDate::fromString( fromUTF8( newValue ), Qt::ISODate ) );
    _qt_dateEdit->blockSignals(false);
}


void YQDateField::setLabel( const string & newLabel )
{
    _caption->setText( fromUTF8( newLabel ) );
    YDateField::setLabel( newLabel );
}


void YQDateField::setEnabled( bool enabled )
{
    QFrame::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQDateField::preferredWidth()
{
    return sizeHint().width();
}


int YQDateField::preferredHeight()
{
    return sizeHint().height();
}


void YQDateField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQDateField::setKeyboardFocus()
{
    _qt_dateEdit->setFocus();

    return true;
}

void YQDateField::changed ( const QDate& )
{
    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}
