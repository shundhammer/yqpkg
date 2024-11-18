/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2019 SUSE LLC
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

  File:	      YQRichText.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <QScrollBar>
#include <QRegExp>
#include <QDebug>
#include <QKeyEvent>
#include <QVBoxLayout>

#include <yui/YApplication.h>
#include <yui/YEvent.h>
#include "utf8.h"
#include "QY2Styler.h"
#include "YQUI.h"
#include "YQDialog.h"
#include "YQRichText.h"

using std::string;
using std::endl;


static const char *colors[] = { "red", "blue", "green", 0};


YQRichText::YQRichText( YWidget *       parent,
                        const string &  text,
                        bool            plainTextMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YRichText( parent, text, plainTextMode )
    , _colors_specified( 0 )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 0 );
    setLayout( layout );

    setWidgetRep( this );

    layout->setMargin( YQWidgetMargin );

    _textBrowser = new YQTextBrowser( this );
    YUI_CHECK_NEW( _textBrowser );
    layout->addWidget( _textBrowser );

    _textBrowser->installEventFilter( this );

    if ( plainTextMode )
    {
        _textBrowser->setWordWrapMode( QTextOption::NoWrap );
    }
    else
    {
        QString style = "\n" + QY2Styler::styler()->textStyle();
        size_t ccolors = sizeof( colors ) / sizeof( char* ) - 1;
        _colors_specified = new bool[ccolors];
        for ( size_t i = 0; i < ccolors; ++i )
        {
            _colors_specified[i] = false;
            char buffer[20];
            sprintf( buffer, "\n.%s ", colors[i] );
            if ( style.contains( buffer ) )
                _colors_specified[i] = true;
        }
        _textBrowser->document()->setDefaultStyleSheet( style );
    }

    setValue( text );

    // Propagate clicks on hyperlinks

    connect( _textBrowser, &pclass(_textBrowser)::anchorClicked,
	     this,	   &pclass(this)::linkClicked );
}


YQRichText::~YQRichText()
{
    // NOP
}


// Update the style for the RichText *content*

void YQRichText::setRichTextStyleSheet( const QString & style )
{
    yuiMilestone() << "Updating RichText stylesheet for " << this << endl;
    _textBrowser->document()->setDefaultStyleSheet( style );

    // unfortunately simple repaint() does not work here :-(
    // so clear the content and set it back agin...
    string scroll = vScrollValue();
    string text = value();
    _textBrowser->document()->clear();
    setValue(text);
    setVScrollValue( scroll );
}


void YQRichText::setValue( const string & newText )
{
    if ( _textBrowser->horizontalScrollBar() )
	_textBrowser->horizontalScrollBar()->setValue( _textBrowser->horizontalScrollBar()->minimum() );

    if ( ! autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->minimum() );

    QString text = fromUTF8( newText );

    if ( ! plainTextMode() )
    {
        for ( int counter = 0; colors[counter]; counter++ )
        {
            if ( !_colors_specified[counter] ) continue;
            text.replace( QString( "color=%1" ).arg( colors[counter] ), QString( "class=\"%1\"" ).arg( colors[counter] ) );
            text.replace( QString( "color=\"%1\"" ).arg( colors[counter] ), QString( "class=\"%1\"" ).arg( colors[counter] ));
        }
        text.replace( "&product;", fromUTF8( YUI::app()->productName() ) );
        _textBrowser->setHtml( text );
    }
    else
    {
        _textBrowser->setPlainText( text );
    }
    YRichText::setValue( newText );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maximum() );
}


void YQRichText::setPlainTextMode( bool newPlainTextMode )
{
    YRichText::setPlainTextMode( newPlainTextMode );

    if ( plainTextMode() )
    {
        _textBrowser->setWordWrapMode( QTextOption::NoWrap );
    }
}


void YQRichText::setAutoScrollDown( bool newAutoScrollDown )
{
    YRichText::setAutoScrollDown( newAutoScrollDown );

    if ( autoScrollDown() && _textBrowser->verticalScrollBar() )
	_textBrowser->verticalScrollBar()->setValue( _textBrowser->verticalScrollBar()->maximum() );
}


void YQRichText::activateLink( const string & url )
{
    YQUI::ui()->sendEvent( new YMenuEvent( url ) );
}


void YQRichText::linkClicked( const QUrl & url )
{
    // yuiDebug() << "Selected hyperlink \"" << url.toString() << "\" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( url.toString().toUtf8()) );
}


bool YQRichText::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * event = ( QKeyEvent * ) ev;

	if ( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
	     ( event->modifiers() & Qt::NoModifier || event->modifiers() & Qt::KeypadModifier ) &&
	     ! haveHyperLinks() )
	{
	    YQDialog * dia = (YQDialog *) findDialog();

	    if ( dia )
	    {
		( void ) dia->activateDefaultButton();
		return true;
	    }
	}
    }

    return QWidget::eventFilter( obj, ev );
}


bool YQRichText::haveHyperLinks()
{
    if ( plainTextMode() )
	return false;

    return ( _textBrowser->document()->toPlainText().contains( QRegExp( "<a\\s+href\\s*=", Qt::CaseInsensitive ) ) > 0 );
}


int YQRichText::preferredWidth()
{
    return shrinkable() ? 10 : 100;
}


int YQRichText::preferredHeight()
{
    return shrinkable() ? 10 : 100;
}


void YQRichText::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQRichText::setEnabled( bool enabled )
{
    _textBrowser->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQRichText::setKeyboardFocus()
{
    _textBrowser->setFocus();

    return true;
}


string YQRichText::vScrollValue() const
{
    return scrollValue( _textBrowser->verticalScrollBar() );
}


void YQRichText::setVScrollValue( const string & newValue )
{
    setScrollValue( _textBrowser->verticalScrollBar(), newValue );
}


string YQRichText::hScrollValue() const
{
    return scrollValue( _textBrowser->horizontalScrollBar() );
}


void YQRichText::setHScrollValue( const string & newValue )
{
    setScrollValue( _textBrowser->horizontalScrollBar(), newValue );
}


string YQRichText::scrollValue( QScrollBar* scrollBar ) const
{
    if ( !scrollBar )
        return "";

    QString tmp;
    tmp.setNum( scrollBar->value() );
    return tmp.toStdString();
}


void YQRichText::setScrollValue( QScrollBar* scrollBar, const string & newValue )
{
    if ( !scrollBar || newValue.empty() )
        return;

    if ( newValue == "minimum" )
        scrollBar->setValue( scrollBar->minimum() );
    else if ( newValue == "maximum" )
        scrollBar->setValue( scrollBar->maximum() );
    else
    {
        QString tmp = QString::fromStdString( newValue );
        scrollBar->setValue( tmp.toInt() );
    }
}


void YQTextBrowser::setSource( const QUrl & name )
{
    // scroll to link if it's available in the current document
    // but prevent loading empty pages

    if ( name.toString().startsWith("#") )
	scrollToAnchor( name.toString().mid(1) );
}
