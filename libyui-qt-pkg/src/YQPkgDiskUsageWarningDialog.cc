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
  File:	      YQPkgDiskUsageWarningDialog.cc
  Author:     Stefan Hundhammer <shundhammer.de>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>

#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgDiskUsageList.h"
#include "QY2LayoutUtils.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgDiskUsageWarningDialog::YQPkgDiskUsageWarningDialog( QWidget *		parent,
							  const QString & 	message,
							  int			thresholdPercent,
							  const QString &	acceptButtonLabel,
							  const QString &	rejectButtonLabel )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Disk Space Warning" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog ( can't simply insert a QVBox )

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    layout->setSpacing( SPACING );
    layout->setMargin ( MARGIN  );
    setLayout(layout);

    // HBox for icon and message
    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    layout->addLayout( hbox );


    // Icon

    //addHSpacing( hbox );
    QLabel * iconLabel = new QLabel( this );
    Q_CHECK_PTR( iconLabel );
    hbox->addWidget(iconLabel);
    iconLabel->setPixmap( YQUI::ui()->loadIcon( "dialog-warning" ).pixmap(64) );
    iconLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert

    // Label for the message

    QLabel * label = new QLabel( message, this);
    Q_CHECK_PTR( label );
    hbox->addWidget(label);
    label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
    label->setTextFormat( Qt::RichText );
    label->setWordWrap( true );


    // Disk usage list

    YQPkgDiskUsageList * duList = new YQPkgDiskUsageList( this, thresholdPercent );
    Q_CHECK_PTR( duList );
    layout->addWidget( duList );


    // Button box

    hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );

    //addHStretch( hbox );


    // Accept button - usually "OK" or "Continue"

    QPushButton * button = new QPushButton( acceptButtonLabel, this );
    Q_CHECK_PTR( button );
    hbox->addWidget(button);

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    //addHStretch( hbox );


    if ( ! rejectButtonLabel.isEmpty() )
    {
	// Reject button ( if desired ) - usually "Cancel"

	button = new QPushButton( rejectButtonLabel, this );
	Q_CHECK_PTR( button );
  hbox->addWidget(button);

	connect( button,	SIGNAL( clicked() ),
		 this,      	SLOT  ( reject()  ) );

	//addHStretch( hbox );
    }

    // If there is only one button, it's safe to make that one ( the accept
    // button ) the default. If there are two, better be safe than sorry and
    // make the reject button the default.

    button->setDefault( true );
}


bool
YQPkgDiskUsageWarningDialog::diskUsageWarning( const QString & 	message,
					       int		thresholdPercent,
					       const QString &	acceptButtonLabel,
					       const QString &	rejectButtonLabel )
{
    YQPkgDiskUsageWarningDialog dialog( 0,
					message,
					thresholdPercent,
					acceptButtonLabel,
					rejectButtonLabel );
    YQUI::ui()->normalCursor();
    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}




