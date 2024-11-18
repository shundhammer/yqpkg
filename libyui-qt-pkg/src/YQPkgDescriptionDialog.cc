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
  File:	      YQPkgDescriptionDialog.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

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
    YQUI::ui()->busyCursor();
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

    YQUI::ui()->normalCursor();
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




