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


#include <zypp/ZYppFactory.h>

#include "Logger.h"
#include "Exception.h"
#include "YQi18n.h"
#include "YQPkgRepoManager.h"


YQPkgRepoManager::YQPkgRepoManager()
{
    logDebug() << "Creating YQPkgRepoManager" << endl;
}


YQPkgRepoManager::~YQPkgRepoManager()
{
    logDebug() << "Destroying YQPkgRepoManager..." << endl;

    shutdownZypp();

    logDebug() << "Destroying YQPkgRepoManager done" << endl;
}


void YQPkgRepoManager::initTarget()
{
    logDebug() << "Initializing zypp..." << endl;

    zyppPtr()->initializeTarget( "/", false );  // don't rebuild rpmdb
    zyppPtr()->target()->load(); // Load pkgs from the target (rpmdb)

    logDebug() << "Initializing zypp done" << endl;
}


void YQPkgRepoManager::shutdownZypp()
{
    logDebug() << "Shutting down zypp..." << endl;

    _repo_manager_ptr.reset();  // deletes the RepoManager
    _zypp_ptr.reset();          // deletes the ZYpp instance

    logDebug() << "Shutting down zypp done" << endl;
}


zypp::ZYpp::Ptr
YQPkgRepoManager::zyppPtr()
{
    if ( ! _zypp_ptr )
        _zypp_ptr = zyppConnectInternal();

    return _zypp_ptr;
}


RepoManager_Ptr
YQPkgRepoManager::repoManager()
{
    if ( ! _repo_manager_ptr )
    {
        logDebug() << "Creating RepoManager" << endl;
        _repo_manager_ptr.reset( new zypp::RepoManager() );
    }

    return _repo_manager_ptr;
}



void YQPkgRepoManager::zyppConnect( int attempts, int waitSeconds )
{
    (void) zyppConnectInternal( attempts, waitSeconds );
}


//
// Stolen from yast-pkg-bindings/src/PkgFunctions.cc
//

zypp::ZYpp::Ptr
YQPkgRepoManager::zyppConnectInternal( int attempts, int waitSeconds )
{
    while ( _zypp_ptr == NULL && attempts > 0 )
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
	    if ( attempts == 1 )  // last attempt?
		ZYPP_RETHROW( ex );
	}

	attempts--;

	if ( _zypp_ptr == NULL && attempts > 0 )
	    sleep( waitSeconds );
    }

    if ( _zypp_ptr == NULL )
    {
	// Still not initialized; throw an exception.
	THROW( Exception( "Can't connect to the package manager" ) );
    }

    return _zypp_ptr;
}


void YQPkgRepoManager::attachRepos()
{
    logDebug() << "TO DO: Load the repos" << endl;
}

