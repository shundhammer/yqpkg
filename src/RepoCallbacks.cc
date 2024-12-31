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


#include "RepoCallbacks.h"


RepoSignalForwarder * RepoSignalForwarder::_instance = 0;


RepoSignalForwarder * RepoSignalForwarder::instance()
{
    if ( ! _instance )
    {
        _instance = new RepoSignalForwarder();
        CHECK_NEW( _instance );
    }

    return _instance;
}


void RepoSignalForwarder::reset()
{
    _doAbort = false;
}


void RepoSignalForwarder::connectRefreshSignals( QObject * receiver )
{
    connect( instance(), SIGNAL( repoRefreshStart   ( QString ) ),
             receiver,   SLOT  ( repoRefreshStart   ( QString ) ) );

    connect( instance(), SIGNAL( repoRefreshProgress( int     ) ),
             receiver,   SLOT  ( repoRefreshProgress( int     ) ) );

    connect( instance(), SIGNAL( repoRefreshEnd() ),
             receiver,   SLOT  ( repoRefreshEnd() ) );
}


//
//----------------------------------------------------------------------
//


RepoRefreshCallbacks::RepoRefreshCallbacks()
{
    logDebug() << "Connecting repo refresh callbacks" << endl;
   _zyppRepoRefreshCallback.connect();
}

RepoRefreshCallbacks::~RepoRefreshCallbacks()
{
    logDebug() << "Disconnecting repo refresh callbacks" << endl;

    // Each zypp::Callback disconnects automatically in the dtor.
}

