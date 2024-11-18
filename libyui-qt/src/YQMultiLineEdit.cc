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

  File:	      YQMultiLineEdit.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QVBoxLayout>
#include <QTextEdit>
#include <qlabel.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQMultiLineEdit.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"

using std::string;



YQMultiLineEdit::YQMultiLineEdit( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMultiLineEdit( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_textEdit = new QTextEdit( this );
    YUI_CHECK_NEW( _qt_textEdit );
    layout->addWidget( _qt_textEdit );

    _qt_textEdit->setAcceptRichText( false );
    _qt_textEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    _caption->setBuddy( _qt_textEdit );

    connect( _qt_textEdit,	&pclass(_qt_textEdit)::textChanged,
	     this, 		&pclass(this)::changed );
}


YQMultiLineEdit::~YQMultiLineEdit()
{
    // NOP
}


string YQMultiLineEdit::value()
{
    return toUTF8( _qt_textEdit->document()->toPlainText() );
}


void YQMultiLineEdit::setValue( const string & text )
{
    YQSignalBlocker sigBlocker( _qt_textEdit );

    _qt_textEdit->setText( fromUTF8( text ) );
}


void YQMultiLineEdit::setLabel( const string & label )
{
    _caption->setText( label );
    YMultiLineEdit::setLabel( label );
}


void YQMultiLineEdit::setInputMaxLength( int newMaxLength )
{
    YMultiLineEdit::setInputMaxLength( newMaxLength );

    QString text = _qt_textEdit->document()->toPlainText();

    if ( (int) text.length() > inputMaxLength() )
    {
	text.truncate( inputMaxLength() );
	_qt_textEdit->setText(text);
    }
}


void YQMultiLineEdit::enforceMaxInputLength()
{
    if ( inputMaxLength() >= 0 && _qt_textEdit->toPlainText().length() > inputMaxLength() )
        _qt_textEdit->undo();
}


void YQMultiLineEdit::changed()
{
    enforceMaxInputLength();

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQMultiLineEdit::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_textEdit->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQMultiLineEdit::preferredWidth()
{
    return std::max( 30, sizeHint().width() );
}


int YQMultiLineEdit::preferredHeight()
{
    int hintHeight 	 = defaultVisibleLines() * _qt_textEdit->fontMetrics().lineSpacing();
    hintHeight		+= _qt_textEdit->frameWidth() * 2 + YQWidgetMargin * 2;

    if ( !_caption->isHidden() )
	hintHeight += _caption->sizeHint().height() + YQWidgetSpacing;

    return std::max( 10, hintHeight );
}


void YQMultiLineEdit::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQMultiLineEdit::setKeyboardFocus()
{
    _qt_textEdit->setFocus();

    return true;
}




