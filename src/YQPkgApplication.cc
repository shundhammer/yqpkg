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

#include "YQi18n.h"
#include "YQPackageSelector.h"
#include "YQPkgApplication.h"
#include "Logger.h"
#include "Exception.h"


YQPkgApplication * YQPkgApplication::_instance = 0;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _pkgSel(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    initZypp();
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
    {
        delete _pkgSel;
        _pkgSel = 0;
    }

    shutdownZypp();
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


void YQPkgApplication::initZypp()
{
    logDebug() << "Initializing zypp..." << endl;

    zyppPtr()->initializeTarget( "/", false );  // don't rebuild rpmdb
    zyppPtr()->target()->load(); // Load pkgs from the target (rpmdb)

    loadRepos();

    logDebug() << "Initializing zypp done" << endl;
}


void YQPkgApplication::shutdownZypp()
{
    logDebug() << "Shutting down zypp..." << endl;

    _repo_manager_ptr.reset();  // deletes the RepoManager
    _zypp_ptr.reset();          // deletes the ZYpp instance

    logDebug() << "Shutting down zypp done" << endl;
}


//
// Stolen from yast-pkg-bindings/src/PkgFunctions.cc
//

zypp::ZYpp::Ptr
YQPkgApplication::zyppPtr()
{
    if ( _zypp_ptr )
	return _zypp_ptr;

    int maxCount = 5;
    unsigned int waitSeconds = 3;

    while ( _zypp_ptr == NULL && maxCount > 0 )
    {
	try
	{
	    logInfo() << "Initializing Zypp library..." << endl;
	    _zypp_ptr = zypp::getZYpp();

 	    // initialize solver flag, be compatible with zypper
	    _zypp_ptr->resolver()->setIgnoreAlreadyRecommended( true );

	    return _zypp_ptr;
	}
	catch ( const zypp::Exception & ex )
	{
	    if ( maxCount == 1 )  // last attempt?
		ZYPP_RETHROW( ex );
	}

	maxCount--;

	if ( _zypp_ptr == NULL && maxCount > 0 )
	    sleep( waitSeconds );
    }

    if ( _zypp_ptr == NULL )
    {
	// Still not initialized; throw an exception.
	// Translators: This is an error message
	THROW( Exception( _( "Cannot connect to the package manager" ) ) );
    }

    return _zypp_ptr;
}


RepoManager_Ptr
YQPkgApplication::repoManager()
{
    if ( ! _repo_manager_ptr )
    {
        logDebug() << "Creating RepoManager" << endl;
        _repo_manager_ptr.reset( new zypp::RepoManager() );
    }

    return _repo_manager_ptr;
}


void YQPkgApplication::loadRepos()
{
    repoManager();
}
