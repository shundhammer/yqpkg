/*
  Copyright (c) 2000-2012 Novell, Inc
  Copyright (c) 2022 SUSE LLC
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

  File:		QY2RelNotesDialog.cc

  Author:	Jiri Srain <jsrain@suse.de>

  Textdomain	"qt"

/-*/

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "QY2RelNotesDialog.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "QY2Styler.h"

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>


#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "qt"


QY2RelNotesDialog::QY2RelNotesDialog( QWidget *parent )
    : QDialog( parent )
{
    QVBoxLayout    *vboxLayout;
    QHBoxLayout    *hboxLayout;

    yuiMilestone() << "Creating YQWizard-internal release notes dialog" << endl;

    if ( objectName().isEmpty() )
        setObjectName( "QRelNotesDialog" );

    resize(581, 388); // same size as help pop-up, proven over time
    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setObjectName( "vboxLayout");

    _tabBar = new QTabBar( this );
    Q_CHECK_PTR( _tabBar );

    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    setFocusProxy( _tabBar );
    setFocusPolicy( Qt::TabFocus );

    connect( _tabBar, SIGNAL( currentChanged ( int ) ),
             this,    SLOT  ( tabChanged     ( int ) ) );

    vboxLayout->addWidget(_tabBar);

    _textBrowser = new QY2TextBrowser(this);
    _textBrowser->setObjectName( "textBrowser");

    vboxLayout->addWidget(_textBrowser);

    hboxLayout = new QHBoxLayout();
    hboxLayout->addStretch();

    _closeButton = new QPushButton(this);
    _closeButton->setObjectName( "closeButton" );
    // Close button for wizard help window
    _closeButton->setText( _( "&Close" ) );

    connect( _closeButton, SIGNAL( clicked() ),
             this,         SLOT  ( accept()  ) );

    hboxLayout->addWidget(_closeButton);
    hboxLayout->addStretch();

    vboxLayout->addLayout(hboxLayout);

    YQUI::setTextdomain( TEXTDOMAIN );

    // Window title for help wizard window
    setWindowTitle( _( "Release Notes" ) );

    QY2Styler::styler()->registerWidget( this );
    // the stylesheet for the displayed text is set separately
    _textBrowser->document()->setDefaultStyleSheet( QY2Styler::styler()->textStyle() );
}


void QY2RelNotesDialog::setRelNotes( const std::map<string,string>& relnotes )
{
    while ( _tabBar->count() > 0 )
    {
        _tabBar->removeTab( 0 );
    }

    _relNotes = relnotes;
    _tabKeys = std::vector<string>();

    for ( std::map<string,string>::const_iterator it = relnotes.begin(); it != relnotes.end(); ++it )
    {
        _tabKeys.push_back(it->first);
        _tabBar->addTab( it->first.c_str() );
    }

    _tabBar->setVisible( _relNotes.size() > 1 );
    _textBrowser->setText( relnotes.begin()->second.c_str() );
}


QY2RelNotesDialog::~QY2RelNotesDialog()
{
    QY2Styler::styler()->unregisterWidget( this );
}


void QY2RelNotesDialog::tabChanged( int index )
{
    if ( index < 0 || _tabKeys.empty() || _relNotes.empty() )
        return;

    _textBrowser->setText( _relNotes[ _tabKeys[ index ] ].c_str() );
}


void QY2RelNotesDialog::retranslate()
{
    setWindowTitle( _( "Release Notes" ) );
    _closeButton->setText( _( "&Close" ) );
}


void QY2TextBrowser::setSource( const QUrl & name )
{
    // scroll to link if it's available in the current document
    // but prevent loading empty pages (bsc#1195158)

    if ( name.toString().startsWith("#") )
	scrollToAnchor( name.toString().mid(1) );
}
