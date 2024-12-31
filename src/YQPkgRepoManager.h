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

class ProgressDialog;


/**
 * Handler for zypp Repos on the YQPkg side
 **/
class YQPkgRepoManager: public QObject
{
    Q_OBJECT
    
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


public slots:

    //
    // Callbacks for repo refresh
    //

    void repoRefreshStart   ( const QString & repoName );
    void repoRefreshProgress( int percent );
    void repoRefreshEnd();


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
     * Return the (non-modal!) repo refresh / load progress dialog.
     * Create it if it doesn't exist yet.
     **/
    ProgressDialog * progressDialog();

    /**
     * Process the pending Qt events.
     *
     * This is necessary in callbacks to receive user events (e.g. button
     * clicks) and to update the display.
     *
     * DO NOT use MainWindow::processEvents() instead which might ignore user
     * input events (e.g. clicks on buttons).
     **/
    void processEvents();


    //
    // Data members
    //

    zypp::ZYpp::Ptr     _zypp_ptr;
    RepoManager_Ptr     _repo_manager_ptr;
    RepoInfoList        _repos;
    ProgressDialog *    _progressDialog;
};

#endif // YQPkgRepoManager_h
