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


#include "QY2CursorHelper.h"
#include "YQi18n.h"
#include "utf8.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QList>
#include <QBoxLayout>

#include "YQPkgDescriptionDialog.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgList.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgDescriptionDialog::YQPkgDescriptionDialog( QWidget * parent, const QString & pkgName )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Package Description" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout(layout);
    layout->setMargin(MARGIN);
    layout->setSpacing(SPACING);



    // VBox for splitter

    QSplitter * splitter = new QSplitter( Qt::Vertical, this );
    Q_CHECK_PTR( splitter );
    layout->addWidget( splitter );

    // Pkg list

    _pkgList = new YQPkgList( splitter );
    Q_CHECK_PTR( _pkgList );
    _pkgList->resize( _pkgList->width(), 80 );


    // Description view

    _pkgDescription = new YQPkgDescriptionView( splitter );
    Q_CHECK_PTR( _pkgDescription );
    _pkgDescription->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,		SIGNAL( currentItemChanged    ( ZyppSel ) ),
	     _pkgDescription,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    // Button box (to center the single button)

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );

    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK" ), this );
    Q_CHECK_PTR( button );
    hbox->addWidget(button);
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    hbox->addStretch();


    filter( pkgName );
}


void
YQPkgDescriptionDialog::filter( const QString & qPkgName )
{
    std::string pkgName( toUTF8( qPkgName ) );
    busyCursor();
    _pkgList->clear();


    // Search for pkgs with that name

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppObj zyppObj = (*it)->theObj();

	if ( zyppObj && zyppObj->name() == pkgName )
	    _pkgList->addPkgItem( *it, tryCastToZyppPkg( zyppObj ) );
    }

#if FIXME
    // Display description of the first pkg with that name

    YQPkgObjListItem * firstItem = dynamic_cast<YQPkgObjListItem *> ( _pkgList->firstChild() );

    if ( firstItem )
	_pkgDescription->showDetailsIfVisible( firstItem->selectable() );
    else
	_pkgDescription->clear();
#endif

    normalCursor();
}


bool
YQPkgDescriptionDialog::isEmpty() const
{
#if FIXME
    return _pkgList->childCount() == 0;
#else
    return true;
#endif
}


QSize
YQPkgDescriptionDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


void
YQPkgDescriptionDialog::showDescriptionDialog( const QString & pkgName )
{
    YQPkgDescriptionDialog dialog( 0, pkgName );
    dialog.exec();
}




