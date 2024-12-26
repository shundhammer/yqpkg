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


#include <unistd.h>             // geteuid()

#include <zypp/ZYppFactory.h>

#include "Logger.h"
#include "Exception.h"
#include "YQi18n.h"
#include "YQPkgApplication.h"
#include "YQPkgRepoManager.h"


YQPkgRepoManager::YQPkgRepoManager()
{
    logDebug() << "Creating YQPkgRepoManager" << Qt::endl;
}


YQPkgRepoManager::~YQPkgRepoManager()
{
    logDebug() << "Destroying YQPkgRepoManager..." << Qt::endl;

    shutdownZypp();

    logDebug() << "Destroying YQPkgRepoManager done" << Qt::endl;
}


void YQPkgRepoManager::initTarget()
{
    logDebug() << "Creating the ZyppLogger" << Qt::endl;
    YQPkgApplication::instance()->createZyppLogger();

    logDebug() << "Initializing zypp..." << Qt::endl;

    zyppPtr()->initializeTarget( "/", false );  // don't rebuild rpmdb
    zyppPtr()->target()->load(); // Load pkgs from the target (rpmdb)

    logDebug() << "Initializing zypp done" << Qt::endl;
}


void YQPkgRepoManager::shutdownZypp()
{
    logDebug() << "Shutting down zypp..." << Qt::endl;

    _repo_manager_ptr.reset();  // deletes the RepoManager
    _zypp_ptr.reset();          // deletes the ZYpp instance

    logDebug() << "Shutting down zypp done" << Qt::endl;
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
        logDebug() << "Creating RepoManager" << Qt::endl;
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
	    logInfo() << "Initializing Zypp library..." << Qt::endl;
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
    // TO DO: Progress callbacks
    // TO DO: check and load services (?)

    findEnabledRepos();
    refreshRepos();
    loadRepos();
}


void YQPkgRepoManager::findEnabledRepos()
{
    for ( zypp::RepoManager::RepoConstIterator it = repoManager()->repoBegin();
          it != repoManager()->repoEnd();
          ++it )
    {
        zypp::RepoInfo repo = *it;

        if ( repo.enabled() )
        {
            _repos.push_back( repo );

            logInfo() << "Found repo \"" << repo.name() << "\""
                      << " URL: " << repo.url().asString()
                      << Qt::endl;
        }
        else
        {
            logInfo() << "Ignoring disabled repo \"" << repo.name() << "\""
                      << Qt::endl;
        }
    }
}


void YQPkgRepoManager::refreshRepos()
{
    if ( geteuid() != 0 )
    {
        logWarning() << "Skipping repos refresh for non-root user" << Qt::endl;
        return;
    }

    // TO DO: Refresh the repos if needed
}


void YQPkgRepoManager::loadRepos()
{
    for ( const zypp::RepoInfo & repo: _repos )
    {
        logDebug() << "Loading resolvables from " << repo.name() << Qt::endl;

        // TO DO: progress callbacks
	repoManager()->loadFromCache( repo );

        // TO DO: This is greatly simplified.
        //
        // See y-pkg-bindings/src/Source_Resolvables.cc for the mind-boggling
        // gory details.
    }
}
