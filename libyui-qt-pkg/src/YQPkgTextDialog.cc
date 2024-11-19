/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


/*
  File:	      YQPkgTextDialog.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include "YQi18n.h"
#include <yui/qt/utf8.h>

#include <QTextBrowser>
#include <QPushButton>
#include <QRegExp>
#include <QLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QEvent>
#include <QApplication>

#include "YQPkgTextDialog.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget

using std::string;


YQPkgTextDialog::YQPkgTextDialog( const QString & text, QWidget * parent )
    : QDialog( parent )
{
    buildDialog( text, parent, _( "&OK" ) );
}


YQPkgTextDialog::YQPkgTextDialog( const QString & 	text,
				  QWidget * 		parent,
				  const QString & 	acceptButtonLabel,
				  const QString & 	rejectButtonLabel )
    : QDialog( parent )
{
    buildDialog( text, parent, acceptButtonLabel, rejectButtonLabel );
}


YQPkgTextDialog::~YQPkgTextDialog()
{
    // NOP
}


void YQPkgTextDialog::buildDialog( const QString & 	text,
				   QWidget * 		parent,
				   const QString & 	acceptButtonLabel,
				   const QString & 	rejectButtonLabel )
{
    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Dialog title
    setWindowTitle( _( "YaST" ) );

    // Layout for the dialog ( can't simply insert a QVBox )

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout( layout );
    layout->setMargin ( MARGIN );
    layout->setSpacing( SPACING );

    // Text browser

    _textBrowser = new QTextBrowser( this );
    Q_CHECK_PTR( _textBrowser );
    layout->addWidget( _textBrowser );
    layout->addSpacing( 2 );
    _textBrowser->document()->setHtml( text );
    _textBrowser->scrollToAnchor( "top" );
    _textBrowser->installEventFilter( this );


    // Button box

    QHBoxLayout * buttonBox = new QHBoxLayout();
    Q_CHECK_PTR( buttonBox );
    buttonBox->setSpacing( SPACING );
    buttonBox->setMargin ( MARGIN  );
    layout->addLayout( buttonBox );
    buttonBox->addStretch();

    // Accept (OK) button

    _acceptButton = new QPushButton( acceptButtonLabel, this );
    buttonBox->addWidget(_acceptButton);
    Q_CHECK_PTR( _acceptButton );
    _acceptButton->setDefault( true );

    connect( _acceptButton,	SIGNAL( clicked() ),
	     this,      	SLOT  ( accept()  ) );

    buttonBox->addStretch();

    if ( ! rejectButtonLabel.isEmpty() )
    {
	// Reject (Cancel) button

	_rejectButton = new QPushButton( rejectButtonLabel, this );
	buttonBox->addWidget(_rejectButton);
	Q_CHECK_PTR( _rejectButton );
	_rejectButton->setDefault( true );

	connect( _rejectButton,	SIGNAL( clicked() ),
		 this,      	SLOT  ( reject()  ) );

	buttonBox->addStretch();
    }
    else
    {
	_rejectButton = 0;
    }

    updateGeometry();
}


QSize
YQPkgTextDialog::sizeHint() const
{
    return limitToScreenSize( this, 500, 450 );
}


bool
YQPkgTextDialog::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev && ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * keyEvent = dynamic_cast<QKeyEvent *> (ev);

	if ( keyEvent )
	{
	    if ( keyEvent->key() == Qt::Key_Return ||
		 keyEvent->key() == Qt::Key_Enter    )
	    {
		_acceptButton->animateClick();
		return true; // Stop event processing
	    }
	    else if ( keyEvent->key() == Qt::Key_Escape )
	    {
		if ( _rejectButton )
		{
		    _rejectButton->animateClick();
		    return true; // Stop event processing
		}
	    }
	}
    }

    return false;	// Don't stop event processing
}


void YQPkgTextDialog::setText( const QString & text )
{
    _textBrowser->document()->setHtml( text );
}


void YQPkgTextDialog::setText( const string & text )
{
    setText( fromUTF8( text ) );
}


void YQPkgTextDialog::setText( ZyppSel selectable,
			       const string & 		 text )
{
    setText( htmlHeading( selectable ) + htmlParagraphs( text ) );
}


void YQPkgTextDialog::showText( QWidget * parent, const QString & text )
{
    YQPkgTextDialog * dia = new YQPkgTextDialog( text, parent );
    Q_CHECK_PTR( dia );
    dia->exec();
    delete dia;
}


void YQPkgTextDialog::showText( QWidget * 	parent,
				ZyppSel 	selectable,
				const string & 	text )
{
    showText( parent, htmlHeading( selectable ) + fromUTF8( text ) );
}


bool YQPkgTextDialog::confirmText( QWidget * 		parent,
				   const QString & 	text,
				   const QString & 	acceptButtonLabel,
				   const QString & 	rejectButtonLabel )
{
    YQPkgTextDialog * dia = new YQPkgTextDialog( text,
						 parent,
						 acceptButtonLabel,
						 rejectButtonLabel );
    Q_CHECK_PTR( dia );
    bool confirmed = ( dia->exec() == QDialog::Accepted );
    delete dia;

    return confirmed;
}


bool YQPkgTextDialog::confirmText( QWidget * parent, const QString & text )
{
    // Translators: "Accept" here refers to licenses or similar
    return confirmText( parent, text, _( "&Accept" ), _( "&Cancel" ) );
}


bool YQPkgTextDialog::confirmText( QWidget * parent, const char * text )
{
    return confirmText( parent, QString( text ) );
}


bool YQPkgTextDialog::confirmText( QWidget * 		parent,
				   ZyppSel 		selectable,
				   const string	&	text )
{
    return confirmText( parent, htmlHeading( selectable ) + htmlParagraphs( text ) );
}





QString
YQPkgTextDialog::htmlEscape( const QString & plainText )
{
    QString html = plainText;
    // logDebug() << "Escaping \"" << plainText << "\"" << endl;

    html.replace( QRegExp( "&" ), "&amp;" );
    html.replace( QRegExp( "<" ), "&lt;"  );
    html.replace( QRegExp( ">" ), "&gt;"  );

    return html;
}



QString
YQPkgTextDialog::htmlParagraphs( const string & rawText )
{
    QString text = fromUTF8( rawText );

    if ( text.contains( "<!-- DT:Rich -->" ) )	// Special doctype for preformatted HTML
	return text;

    text = htmlEscape( text );			// Escape '<', '>', '&'
    text.replace( "\n\n", "</p><p>" );		// Empty lines mean new paragraph
    text.prepend( "<p>"  );
    text.append ( "</p>" );

    return text;
}



QString
YQPkgTextDialog::htmlHeading( const QString & text )
{
    QString html =
	"<table><tr><td><b>"
	+ text
	+ "</b></td></tr></table><br>";

    return html;
}


QString
YQPkgTextDialog::htmlHeading( ZyppSel selectable )
{
    if ( ! selectable )
	return "";

    ZyppObj zyppObj = selectable->theObj();

    if ( ! zyppObj )
	return "";

    QString summary = fromUTF8( zyppObj->summary() );

    QString html =
	"<table><tr><td><b>"
	+ fromUTF8( zyppObj->name() )
	+ "</b>";

    if ( ! summary.isEmpty() )
	html += " - " + summary;

    html += "</td></tr></table><br>";

    return html;
}




