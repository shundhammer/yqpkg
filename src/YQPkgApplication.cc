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

#include "BusyPopup.h"
#include "Exception.h"
#include "Logger.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "YQPkgRepoManager.h"
#include "YQPkgApplication.h"


YQPkgApplication * YQPkgApplication::_instance = 0;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _pkgSel(0)
    , _yqPkgRepoManager(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    attachRepos();
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
        delete _pkgSel;

    detachRepos();
    _instance = 0;

    logDebug() << "Destroying YQPkgApplication done" << endl;
}


void YQPkgApplication::run()
{
    createPkgSel();
    qApp->exec();
}


void YQPkgApplication::createPkgSel()
{
    if ( _pkgSel )
        return;

    BusyPopup busyPopup( _( "Preparing the package selector..." ) );

    _pkgSel = new YQPackageSelector( 0, 0 );
    CHECK_PTR( _pkgSel );

    QObject::connect( _pkgSel, SIGNAL( commit() ),
                      qApp,    SLOT  ( quit()   ) );

    QString windowTitle( "YQPkg" );
    windowTitle += runningAsRoot() ? _( " [root]" ) : _( " (read-only)" );
    _pkgSel->setWindowTitle( windowTitle );
    _pkgSel->show();
}


bool YQPkgApplication::runningAsRoot()
{
    return geteuid() == 0;
}


void YQPkgApplication::attachRepos()
{
    logDebug() << "Initializing zypp..." << endl;

    if ( ! _yqPkgRepoManager )
    {
        _yqPkgRepoManager = new YQPkgRepoManager();
        CHECK_NEW( _yqPkgRepoManager );
    }

    try
    {
        BusyPopup busyPopup( _( "Loading package manager data..." ) );

        _yqPkgRepoManager->zyppConnect(); // This may throw
        _yqPkgRepoManager->initTarget();
        _yqPkgRepoManager->attachRepos();

        // The BusyPopup closes when it goes out of scope
        logDebug() << "Busy Popup closed" << endl;
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
