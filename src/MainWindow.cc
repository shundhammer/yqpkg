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


#include <QCloseEvent>
#include <QVBoxLayout>

#include "Logger.h"
#include "Exception.h"
#include "WindowSettings.h"
#include "MainWindow.h"


MainWindow * MainWindow::_instance = 0;


MainWindow::MainWindow( QWidget * parent )
    : QWidget( parent )
    , _layout(0)
{
    _instance = this;
    basicLayout();
    WindowSettings::read( this, "MainWindow" );
}


MainWindow::~MainWindow()
{
    WindowSettings::write( this, "MainWindow" );

    if ( _emptyPage )
        delete _emptyPage;

    _instance = 0;
}


void MainWindow::basicLayout()
{
    _layout = new QVBoxLayout( this );
    CHECK_NEW( _layout );

    _layout->setAlignment( Qt::AlignCenter );
}


void MainWindow::addPage( QWidget * page, const QString & pageName )
{
    CHECK_PTR( page );

    if ( ! pageName.isEmpty() )
        page->setObjectName( pageName );

    _pages << QWidgetPointer( page );
}


void MainWindow::showPage( QWidget * page )
{
    _currentPage = page; // even if it's 0

    clearClientArea();

    if ( page )
    {
        if ( ! page->objectName().isEmpty() )
        {
            logDebug() << "Showing page " << page->objectName()
                       << " class " << page->metaObject()->className()
                       << endl;
        }

        _layout->addWidget( page );
        _page->show();
    }
}


void MainWindow::clearClientArea()
{
    while ( _layout && _layout->count() > 0 )
    {
        QLayoutItem * layoutItem = _layout->takeAt( 0 );

        if ( layoutItem && layoutItem->widget() )
            layoutItem->widget()->hide();
    }
}


void MainWindow::showPage( const QString & pageName )
{
    QWidget * page = findPage( page );

    if ( page )
        showPage( page );
}


QWidget * MainWindow::findPage( const QString & pageName )
{
    if ( pageName.isEmpty() )
    {
        logError() << "Empty page name" << endl;
        return 0;
    }

    foreach ( QWidgetPointer page, _pages )
    {
        if ( page->objectName() == pageName )
            return page;
    }

    logError() << "No page with name \"" << pageName "\"" << endl;

    return 0;
}


void MainWindow::processEvents( int millisec )
{
    QEventLoop eventLoop;
    eventLoop.processEvents( QEventLoop::ExcludeUserInputEvents,
			     millisec );
}


void MainWindow::closeEvent( QCloseEvent * event )
{
    Q_UNUSED( event );

    logInfo() << "Caught WM_CLOSE" << endl;

    // event->ignore();
}

