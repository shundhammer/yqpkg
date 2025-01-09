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


#ifndef MyrlynRepoManager_h
#define MyrlynRepoManager_h

#include <list>

#include <zypp/ZYpp.h>
#include <zypp/RepoManager.h>
#include <zypp/RepoInfo.h>


typedef boost::shared_ptr<zypp::RepoManager> RepoManager_Ptr;
typedef std::list<zypp::RepoInfo> RepoInfoList;
typedef std::list<zypp::RepoInfo>::iterator RepoInfoIterator;


/**
 * Handler for zypp Repos on the Myrlyn side
 **/
class MyrlynRepoManager: public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     **/
    MyrlynRepoManager();

    /**
     * Destructor
     **/
    virtual ~MyrlynRepoManager();

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
     *
     * Once a connection exists, you can simply use
     *
     *   zypp::getZYpp()
     *
     * (notice the capital 'Y'!).
     **/
    zypp::ZYpp::Ptr zyppPtr();

    /**
     * Return the zypp repo manager. The first call will create it.
     **/
    RepoManager_Ptr repoManager();


signals:

    /**
     * Emitted when an enabled repo was found.
     **/
    void foundRepo( const zypp::RepoInfo & repo );

    /**
     * Emitted when refreshing a repo starts.
     **/
    void refreshRepoStart( const zypp::RepoInfo & repo );

    /**
     * Emitted when refreshing a repo is done.
     **/
    void refreshRepoDone ( const zypp::RepoInfo & repo );


protected:

    /**
     * Try to connect to the package manager (libzypp) and return the new zypp
     * ptr. Throw if this wasn't successful after 'attempts' number of attempts
     * with 'waitSeconds' waiting time between them.
     **/
    zypp::ZYpp::Ptr zyppConnectInternal( int attempts    = 5,
                                         int waitSeconds = 3 );

    /**
     * Shut down all the zypp objects that we created in the correct order.
     **/
    void shutdownZypp();

    /**
     * Find the enabled repos from the zypp repo manager and put them into
     * _repos.
     **/
    void findEnabledRepos();

    /**
     * Refresh the enabled repos if needed.
     * This is skipped for non-privileged users.
     **/
    void refreshRepos();

    /**
     * Load the resolvables from the enabled repos.
     **/
    void loadRepos();

    /**
     * Notify the user to run 'zypper dup' in a warning pop-up and on stderr.
     * This does not exit.
     **/
    void notifyUserToRunZypperDup() const;


    //
    // Data members
    //

    zypp::ZYpp::Ptr _zypp_ptr;
    RepoManager_Ptr _repo_manager_ptr;
    RepoInfoList    _repos;
};

#endif // MyrlynRepoManager_h
