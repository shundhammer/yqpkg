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


#include "YQi18n.h"
#include "QY2CursorHelper.h"

#include <QApplication>
#include <qdesktopwidget.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSplitter>
#include <QStyle>
#include <QTabWidget>
#include <QList>
#include <QBoxLayout>

#include "YQPkgProductDialog.h"
#include "YQPkgProductList.h"
#include "YQPkgDependenciesView.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


#define SPACING			2	// between subwidgets
#define MARGIN			4	// around the widget


YQPkgProductDialog::YQPkgProductDialog( QWidget * parent )
    : QDialog( parent )
{
    // Dialog title
    setWindowTitle( _( "Products" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );
    setMinimumSize( 550, 450 );

    // Layout for the dialog (can't simply insert a QVBox)

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout(layout);
    layout->setSpacing( SPACING );
    layout->setMargin ( MARGIN  );

    // VBox for splitter

    QSplitter * splitter = new QSplitter( Qt::Vertical, this );
    Q_CHECK_PTR( splitter );
    layout->addWidget( splitter );


    // Product list

    _productList = new YQPkgProductList( splitter );
    Q_CHECK_PTR( _productList );
    _productList->resize( _productList->width(), 80 );

    // Prevent status changes for now - this would require solver runs etc.
    _productList->setEditable( false );


    // Tab widget for details views (looks better even just with one)

    _detailsViews = new QTabWidget( splitter );
    Q_CHECK_PTR( _detailsViews );
    //_detailsViews->setMargin( MARGIN );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    // Dependencies view

    _dependenciesView = new YQPkgDependenciesView( _detailsViews );
    Q_CHECK_PTR( _dependenciesView );
    _detailsViews->addTab( _dependenciesView, _( "Dependencies" ) );
    _dependenciesView->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _productList,	SIGNAL( currentItemChanged  ( ZyppSel ) ),
	     _dependenciesView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    // Button box (to center the single button)

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setMargin ( MARGIN  );
    layout->addLayout( hbox );
    hbox->addStretch();


    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK" ), this );
    Q_CHECK_PTR( button );
    hbox->addWidget(button);
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    hbox->addStretch();
}


void
YQPkgProductDialog::polish()
{
    // Delayed initialization after widget is fully created etc.

    // Only now send currentItemChanged() signal so the details views display something
    // (showDetailsIfVisible() shows only something if the widget is visible,
    // as the method name implies)
    _productList->selectSomething();
}


QSize
YQPkgProductDialog::sizeHint() const
{
    QRect available = qApp->desktop()->availableGeometry( (QWidget *) this );
    QSize size = QDialog::sizeHint();
    size = size.boundedTo( QSize( available.width(), available.height() ) );

    return size;
}


void
YQPkgProductDialog::showProductDialog( QWidget* parent)
{
    YQPkgProductDialog dialog( parent );
    dialog.exec();
}




