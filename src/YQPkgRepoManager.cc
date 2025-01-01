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


#include <unistd.h>             // geteuid(), sleep()
#include <iostream>             // cerr
#include <QElapsedTimer>
#include <QMessageBox>

#include <zypp/ZYppFactory.h>

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "YQPkgApplication.h"
#include "YQi18n.h"
#include "utf8.h"
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
    logDebug() << "Creating the ZyppLogger" << endl;
    YQPkgApplication::instance()->createZyppLogger();

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
    // TO DO: check and load services (?)

    try
    {
        findEnabledRepos();
        refreshRepos();
        loadRepos();
    }
    catch ( const zypp::Exception & ex )
    {
        logError() << "Caught zypp exception: " << ex.asString() << endl;

        if ( geteuid() != 0 )
        {
            notifyUserToRunZypperDup();

            logInfo() << "Exiting." << endl;
            exit( 1 );
        }

        throw;  // Nothing else that we can do here
    }
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
                      << endl;

            emit foundRepo( repo );
        }
        else
        {
            logInfo() << "Ignoring disabled repo \"" << repo.name() << "\""
                      << endl;
        }
    }
}


void YQPkgRepoManager::refreshRepos()
{
    if ( geteuid() != 0 )
    {
        logWarning() << "Skipping repos refresh for non-root user" << endl;
        return;
    }

    if ( YQPkgApplication::isOptionSet( OptNoRepoRefresh ) )
        return;

    QElapsedTimer timer;

    for ( const zypp::RepoInfo & repo: _repos )
    {
        try
        {
            timer.start();
            logInfo() << "Refreshing repo " << repo.name() << "..." << endl;
            emit refreshRepoStart( repo );

            repoManager()->refreshMetadata( repo, zypp::RepoManager::RefreshIfNeeded );
            repoManager()->buildCache     ( repo, zypp::RepoManager::BuildIfNeeded   );

            if ( YQPkgApplication::isOptionSet( OptSlowRepoRefresh ) )
                sleep( 2 );

            logInfo() << "Refreshing repo " << repo.name()
                      << " done after " << timer.elapsed() / 1000.0 << " sec"
                      << endl;

            emit refreshRepoDone( repo );
        }
        catch ( const zypp::repo::RepoMetadataException & exception )
        {
            Q_UNUSED( exception );
            logWarning() << "CAUGHT zypp exception for repo " << repo.name() << endl;
        }
    }
}


void YQPkgRepoManager::loadRepos()
{
    for ( const zypp::RepoInfo & repo: _repos )
    {
        logDebug() << "Loading resolvables from " << repo.name() << endl;

	repoManager()->loadFromCache( repo );
    }
}


void YQPkgRepoManager::notifyUserToRunZypperDup() const
{
    logInfo() << "Run 'sudo zypper refresh' and restart the program." << endl;

    QString message = _( "Error loading the repos. Run\n\n"
                         "    sudo zypper refresh\n\n"
                         "and restart the program." );
    std::cerr << toUTF8( message ) << std::endl;

    QMessageBox::warning( MainWindow::instance(), // parent
                          _( "Error" ),
                          message,
                          QMessageBox::Ok );
}

