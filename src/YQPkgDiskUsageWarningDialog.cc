/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

#include "Exception.h"
#include "Logger.h"
#include "QY2CursorHelper.h"
#include "QY2IconLoader.h"
#include "QY2LayoutUtils.h"
#include "YQPkgDiskUsageList.h"
#include "YQi18n.h"

#include "YQPkgDiskUsageWarningDialog.h"


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

    QLabel * iconLabel = new QLabel( this );
    Q_CHECK_PTR( iconLabel );
    hbox->addWidget(iconLabel);
    iconLabel->setPixmap( QY2IconLoader::loadIcon( "dialog-warning" ).pixmap(64) );
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
    normalCursor();
    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}




