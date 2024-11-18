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

  File:	      YQLogView.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qlabel.h>
#include <qstyle.h>
#include <QVBoxLayout>
#include <QScrollBar>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQLogView.h"
#include "YQWidgetCaption.h"

using std::string;



YQLogView::YQLogView( YWidget * 	parent,
		      const string &	label,
		      int 		visibleLines,
		      int 		maxLines )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YLogView( parent, label, visibleLines, maxLines )
{
    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_text = new MyTextEdit( this );
    YUI_CHECK_NEW( _qt_text );
    layout->addWidget( _qt_text );

    _qt_text->setReadOnly( true );
    _qt_text->setAcceptRichText( false );
    _qt_text->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    _caption->setBuddy( _qt_text );

    connect (_qt_text, &pclass(_qt_text)::resized, this, &pclass(this)::slotResize);

}


YQLogView::~YQLogView()
{
    // NOP
}


void
YQLogView::displayLogText( const string & text )
{
    QScrollBar *sb = _qt_text->verticalScrollBar();
    QString newString = fromUTF8( text );
    // no change -> no need to update
    if (newString == _lastText)
        return;

    bool atEnd = sb->value() == sb->maximum();

    if (newString.startsWith(_lastText) && !_lastText.isEmpty() )
    {
	int position = _lastText.length();

	// prevent double line break caused by QTextEdit::append()
	if ( newString.mid( _lastText.length(), 1 ) == QString('\n') )
            position++;

        _qt_text->append( newString.mid( position) );
    }
    else
    {
        _qt_text->setPlainText( newString );
    }


    if (atEnd)
    {
        _qt_text->moveCursor( QTextCursor::End );
        _qt_text->ensureCursorVisible();
        sb->setValue( sb->maximum() );
    }

    _lastText = newString;
}


void
YQLogView::setLabel( const string & label )
{
    _caption->setText( label );
    YLogView::setLabel( label );
}


void
YQLogView::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_text->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQLogView::preferredWidth()
{
    return std::max( 50, sizeHint().width() );
}


int
YQLogView::preferredHeight()
{
    int hintHeight 	 = visibleLines() * _qt_text->fontMetrics().lineSpacing();
    hintHeight		+= _qt_text->style()->pixelMetric( QStyle::PM_ScrollBarExtent );
    hintHeight		+= _qt_text->frameWidth() * 2;

    if ( !_caption->isHidden() )
	hintHeight	+=  _caption->sizeHint().height();

    return std::max( 80, hintHeight );
}


void
YQLogView::slotResize()
{
    QScrollBar *sb = _qt_text->verticalScrollBar();

    bool atEnd = sb->value() == sb->maximum();

    if (atEnd)
    {
        _qt_text->moveCursor( QTextCursor::End );
        _qt_text->ensureCursorVisible();
        sb->setValue( sb->maximum() );
    }
}


void
YQLogView::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQLogView::setKeyboardFocus()
{
    _qt_text->setFocus();

    return true;
}
