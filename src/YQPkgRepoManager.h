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


#ifndef YQPkgRepoManager_h
#define YQPkgRepoManager_h

#include <list>

#include <zypp/ZYpp.h>
#include <zypp/RepoManager.h>
#include <zypp/RepoInfo.h>


typedef boost::shared_ptr<zypp::RepoManager> RepoManager_Ptr;
typedef std::list<zypp::RepoInfo> RepoInfoList;
typedef std::list<zypp::RepoInfo>::iterator RepoInfoIterator;


/**
 * Handler for zypp Repos on the YQPkg side
 **/
class YQPkgRepoManager: public QObject
{
public:
    /**
     * Constructor
     **/
    YQPkgRepoManager();

    /**
     * Destructor
     **/
    virtual ~YQPkgRepoManager();

    /**
     * Connect to the package manager (libzypp).
     *
     * This will do several attempts with some seconds of waiting between
     * them. This will throw if a connection cannot be established, for example
     * because the package manager is already busy in another window.
     **/
    void zyppConnect( int attempts    = 5,
                      int waitSeconds = 3);

    /**
     * Initialize the target (the installed system): Add it as a repos
     * ("@System") and load its resovables (packages, patterns etc.) from its
     * RPMDB.
     **/
    void initTarget();

    /**
     * Attach the active repos and load their resolvables.
     **/
    void attachRepos();

    /**
     * Return the connection to zypp.
     * The first call will establish the connection.
     **/
    zypp::ZYpp::Ptr zyppPtr();


    /**
     * Return the zypp repo manager. The first call will create it.
     **/
    RepoManager_Ptr repoManager();


protected:

    /**
     * Try to connect to the package manager (libzypp) and return the new zypp
     * ptr. Throw if this wasn't successful after 'attempts' number of attempts
     * with 'waitSeconds' waiting time between them.
     **/
    zypp::ZYpp::Ptr zyppConnectInternal( int attempts    = 5,
                                         int waitSeconds = 3 );


    void initZypp();
    void shutdownZypp();
    void loadRepos();

    //
    // Data members
    //

    zypp::ZYpp::Ptr     _zypp_ptr;
    RepoManager_Ptr     _repo_manager_ptr;
    RepoInfoList        _repos;
};

#endif // YQPkgRepoManager_h
