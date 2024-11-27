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


#include <unistd.h>     // getuid()

#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>

#include "BusyPopup.h"
#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "YQPkgRepoManager.h"
#include "YQPkgApplication.h"


YQPkgApplication * YQPkgApplication::_instance = 0;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _mainWin(0)
    , _pkgSel(0)
    , _yqPkgRepoManager(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    createMainWin();
    attachRepos();
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
        delete _pkgSel;

    if ( _mainWin )
        delete _mainWin;

    detachRepos();
    _instance = 0;

    logDebug() << "Destroying YQPkgApplication done" << endl;
}


void YQPkgApplication::run()
{
    createPkgSel();
    _mainWin->showPage( _pkgSel );

    qApp->exec();
}


void YQPkgApplication::createMainWin()
{
    if ( _mainWin )
        return;

    _mainWin = new MainWindow();
    CHECK_NEW( _mainWin );

    setWindowTitle( _mainWin );
    _mainWin->installEventFilter( this );
    _mainWin->show();
}


void YQPkgApplication::setWindowTitle( QWidget * window )
{
    if ( window )
    {
        QString windowTitle( "YQPkg" );
        windowTitle += runningAsRoot() ? _( " [root]" ) : _( " (read-only)" );
        window->setWindowTitle( windowTitle );
    }
}


void YQPkgApplication::createPkgSel()
{
    if ( _pkgSel )
        return;

    QLabel busyPage( _( "Preparing the package selector..." ) );

    if ( _mainWin )
        _mainWin->splashPage( &busyPage );

    _pkgSel = new YQPackageSelector( 0, 0 );
    CHECK_PTR( _pkgSel );

    QObject::connect( _pkgSel, SIGNAL( commit() ),
                      qApp,    SLOT  ( quit()   ) );

    _mainWin->addPage( _pkgSel, "pkgSel" );
}


bool YQPkgApplication::runningAsRoot()
{
    return geteuid() == 0;
}


void YQPkgApplication::attachRepos()
{
    logDebug() << "Initializing zypp..." << endl;
    QLabel repoSplashPage( _( "Loading package manager data..." ) );

    if ( _mainWin )
        _mainWin->splashPage( &repoSplashPage );

    if ( ! _yqPkgRepoManager )
    {
        _yqPkgRepoManager = new YQPkgRepoManager();
        CHECK_NEW( _yqPkgRepoManager );
    }

    try
    {
        _yqPkgRepoManager->zyppConnect(); // This may throw
        _yqPkgRepoManager->initTarget();
        _yqPkgRepoManager->attachRepos();
    }
    catch ( ... )
    {
        QString message = _( "Can't connect to the package manager!\n"
                             "It may be busy in another window.\n" );

        QMessageBox::warning( 0, // parent
                              _( "Error" ),
                              message,
                              QMessageBox::Ok );
        throw;
    }

    logDebug() << "Initializing zypp done" << endl;
}


void YQPkgApplication::detachRepos()
{
    if ( _yqPkgRepoManager )
    {
        delete _yqPkgRepoManager;
        _yqPkgRepoManager = 0;
    }
}


bool YQPkgApplication::eventFilter( QObject * watchedObj, QEvent * event )
{
    if ( _mainWin && watchedObj == _mainWin && _pkgSel
	 && event && event->type() == QEvent::Close ) // WM_CLOSE (Alt-F4)
    {
        if ( _mainWin->currentPage() == _pkgSel )
        {
            logInfo() << "Caught WM_CLOSE for YQPackageSelector" << endl;
            _pkgSel->reject();  // _pkgSel handles asking for confirmation etc.

            return true;        // Event processing finished for this one
        }
        else
        {
            logInfo() << "Caught WM_CLOSE, but not for YQPackageSelector" << endl;
        }
    }

    return false; // Event processing not yet finished, continue down the chain
}
