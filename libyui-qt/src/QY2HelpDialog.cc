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

  File:		QY2HelpDialog.cc

  Author:	Stephan Kulow <coolo@suse.de>

  Textdomain	"qt"

/-*/

#include <QTextObject>

#include "QY2HelpDialog.h"
#include "ui_QY2HelpDialog.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "QY2Styler.h"

#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "qt"


QY2HelpDialog::QY2HelpDialog( const QString& helpText, QWidget *parent )
    : QDialog( parent )
    , _searchResultForeground(Qt::black)
    , _searchResultBackground(Qt::yellow)
{
    _ui = new Ui_QY2HelpDialog();
    _ui->setupUi( this );
    _ui->textBrowser->setText( helpText );
    Q_INIT_RESOURCE(qt_icons);
    QIcon icon = QIcon::fromTheme( "edit-find", QIcon( ":/edit-find" ) );
    _ui->label->setPixmap ( icon.pixmap( QSize( 16, 16 ) ) );
    connect( _ui->lineEdit, &pclass(_ui->lineEdit)::textEdited,
             this, &pclass(this)::searchStringChanged );

    _ui->lineEdit->setFocus( Qt::OtherFocusReason );
    _ui->pushButton->setAutoDefault(false);

    YQUI::setTextdomain( TEXTDOMAIN );

    // Window title for help wizard window
    setWindowTitle( _( "Help" ) );

    // Close button for wizard help window
    _ui->pushButton->setText( _( "&Close" ) );

    QY2Styler::styler()->registerWidget( this );
}

void QY2HelpDialog::setHelpText( const QString& helpText )
{
    _ui->textBrowser->setText( helpText );
    _ui->lineEdit->setText( QString() );
    _ui->lineEdit->setFocus( Qt::OtherFocusReason );
}

QY2HelpDialog::~QY2HelpDialog()
{
    QY2Styler::styler()->unregisterWidget( this );
    delete _ui;
}

void QY2HelpDialog::searchStringChanged( QString text )
{
    QTextCharFormat fmt;
    fmt.setBackground(getSearchResultBackground());
    fmt.setForeground(getSearchResultForeground());
    QTextDocument *d = _ui->textBrowser->document();

    QTextCursor all(d);
    all.select ( QTextCursor::Document);
    all.setCharFormat( QTextCharFormat() );

    _marks.clear();

    QTextCursor c( d );

    while ( true )
    {
        c = d->find( text, c );
        if ( c.isNull() )
            break;
        c.setCharFormat( fmt );
        c.select( QTextCursor::WordUnderCursor );
        _marks.push_back( c );
    }
}

void QY2HelpDialog::retranslate()
{
    setWindowTitle( _( "Help" ) );
    _ui->pushButton->setText( _( "&Close" ) );
}


QColor QY2HelpDialog::getSearchResultForeground()
{
    return _searchResultForeground;
}

void QY2HelpDialog::setSearchResultForeground( QColor pen )
{
    _searchResultForeground = pen;
}

QColor QY2HelpDialog::getSearchResultBackground()
{
    return _searchResultBackground;
}

void QY2HelpDialog::setSearchResultBackground( QColor pen )
{
    _searchResultBackground = pen;
}




