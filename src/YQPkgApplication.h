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


#ifndef YQPkgApplication_h
#define YQPkgApplication_h

#include <list>
#include <QObject>

#include <zypp/ZYpp.h>
#include <zypp/RepoManager.h>
#include <zypp/RepoInfo.h>


class YQPackageSelector;

typedef boost::shared_ptr<zypp::RepoManager> RepoManager_Ptr;
typedef std::list<zypp::RepoInfo> RepoInfoList;
typedef std::list<zypp::RepoInfo>::iterator RepoInfoIterator;


/**
 * Application class for yqpkg.
 **/
class YQPkgApplication: public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     **/
    YQPkgApplication();

    /**
     * Destructor
     **/
    virtual ~YQPkgApplication();

    /**
     * Return the instance of this class or 0 if there is none.
     *
     * This is not a real singleton, but for the lifetime of this application
     * this instance will remain alive, i.e. for most other classes related to
     * this.
     **/
    static YQPkgApplication * instance() { return _instance; }

    /**
     * Run the application. This also handles the Qt event loop.
     **/
    void run();

    /**
     * Return 'true' if this program is running with root privileges.
     **/
    static bool runningAsRoot();

    /**
     * Return the connection to zypp.
     * The first call will establish the connection.
     **/
    zypp::ZYpp::Ptr zyppPtr();


    /**
     * Return the repo manager. The first call will create it.
     **/
    RepoManager_Ptr repoManager();


protected:

    /**
     * Create (and show) the YQPackageSelector if it doesn't already exist.
     **/
    void createPkgSel();

    void initZypp();
    void shutdownZypp();
    void loadRepos();

    //
    // Data members
    //

    YQPackageSelector * _pkgSel;

    zypp::ZYpp::Ptr     _zypp_ptr;
    RepoManager_Ptr     _repo_manager_ptr;
    RepoInfoList        _repos;

    static YQPkgApplication * _instance;
};

#endif // YQPkgApplication_h
