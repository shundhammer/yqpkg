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

#include <QObject>
#include <zypp/ZYpp.h>


class MainWindow;
class YQPackageSelector;
class YQPkgRepoManager;
class QEvent;

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


protected:

    /**
     * Create and set up the main window.
     **/
    void createMainWin();

    /**
     * Set the appropriate window title, depending if the applicaton is running
     * with or without root permissions.
     **/
    void setWindowTitle( QWidget * window );

    /**
     * Create (and show) the YQPackageSelector if it doesn't already exist.
     **/
    void createPkgSel();

    /**
     * Initialize and attach the repos:
     *
     *   - Create the YQPkgRepoManager
     *   - Connect to libzypp
     *   - initialize the target (load the resolvables from the RPMDB)
     *   - attach all active repos
     **/
    void attachRepos();

    /**
     * Shut down and detach the repos.
     * This also destroys the YQPkgRepoManager.
     **/
    void detachRepos();

    /**
     * Event filter to catch foreign events, e.g. the MainWindow WM_CLOSE.
     *
     * Reimplemented from QObject.
     **/
    virtual bool eventFilter( QObject * watchedObj, QEvent * event ) override;


    //
    // Data members
    //

    MainWindow *        _mainWin;
    YQPackageSelector * _pkgSel;
    YQPkgRepoManager  * _yqPkgRepoManager;

    static YQPkgApplication *   _instance;
    static bool                 _fakeRoot;      // env YQPKG_FAKE_ROOT
};

#endif // YQPkgApplication_h
