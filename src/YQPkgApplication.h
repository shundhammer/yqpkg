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
class PkgCommitPage;
class PkgTasks;
class QEvent;
class SummaryPage;
class Workflow;
class YQPkgSelector;
class YQPkgRepoManager;
class ZyppLogger;

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

    //
    // Access to some important member variables
    //

    Workflow *   workflow() const { return _workflow; }
    MainWindow * mainWin()  const { return _mainWin; }

    /**
     * Return the package selector. Create it if it doesn't exist yet.
     *
     * Ownership remains with this class; do not delete it.
     **/
    YQPkgSelector * pkgSel();

    /**
     * Return the package committer. Create it if it doesn't exist yet.
     *
     * Ownership remains with this class; do not delete it.
     **/
    PkgCommitPage * pkgCommitPage();

    /**
     * Return the summary page. Create it if it doesn't exist yet.
     *
     * Ownership remains with this class; do not delete it.
     **/
    SummaryPage * summaryPage();

    /**
     * Return the package tasks object. Create it if it doesn't exist yet.
     *
     * Ownership remains with this class; do not delete it.
     **/
    PkgTasks * pkgTasks();

    /**
     * Return the YQPkgRepoManager. Create it if it doesn't exist yet.
     *
     * Ownership remains with this class; do not delete it.
     **/
    YQPkgRepoManager * repoManager();

    /**
     * Return the ZyppLogger. Create it if it doesn't exist yet.
     **/
    ZyppLogger * zyppLogger();

    /**
     * Create the ZyppLogger if it doesn't exist yet.
     **/
    void createZyppLogger();


public slots:

    /**
     * Go to the next worflow step.
     * If there is no more step, quit the program.
     **/
    void next();

    /**
     * Go to the previous worflow step.
     **/
    void back();

    /**
     * Quit the program when the last workflow step was reached.
     **/
    void finish();

    /**
     * Restart the workflow: Go back to the package selection.
     **/
    void restart();

    /**
     * The user finished the package selection with "Accept", but there was no
     * change: Skip the "commit" phase and go traight to the summary screen.
     *
     * Alternatively this could simply quit the workflow.
     **/
    void skipCommit();

    /**
     * Quit the program.
     *
     * Ask for confirmation if 'askForConfirmation' is 'true'.
     **/
    void quit( bool askForConfirmation = false );


protected:

    // Create the various objects (and set up Qt connections if necessary)
    // if they don't exist yet.

    void createMainWin();
    void createWorkflow();
    void createPkgSel();
    void createPkgCommitPage();
    void createSummaryPage();
    void createPkgTasks();
    void createRepoManager();

    /**
     * Set the appropriate window title, depending if the applicaton is running
     * with or without root permissions.
     **/
    void setWindowTitle( QWidget * window );

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
    Workflow *          _workflow;
    YQPkgSelector *     _pkgSel;
    PkgCommitPage *     _pkgCommitPage;
    SummaryPage *       _summaryPage;
    YQPkgRepoManager  * _yqPkgRepoManager;
    ZyppLogger *        _zyppLogger;
    PkgTasks *          _pkgTasks;

    static YQPkgApplication *   _instance;
    static bool                 _fakeRoot;      // env YQPKG_FAKE_ROOT
};

#endif // YQPkgApplication_h
