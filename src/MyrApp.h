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


#ifndef MyrApp_h
#define MyrApp_h

#include <QObject>
#include <QFlags>
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

typedef std::list<zypp::RepoInfo> RepoInfoList;
typedef std::list<zypp::RepoInfo>::iterator RepoInfoIterator;


enum YQPkgAppOption
{
    OptNone            = 0,
    OptReadOnly        = 0x01,
    OptDryRun          = 0x02,
    OptDownloadOnly    = 0x04,
    OptNoRepoRefresh   = 0x08,

    // For debugging

    OptFakeRoot        = 0x100,
    OptFakeCommit      = 0x200,
    OptFakeSummary     = 0x400,
    OptSlowRepoRefresh = 0x800,
};

// See https://doc.qt.io/qt-5/qflags.html
//
// YQPkgAppOption  is just the enum,
// YQPkgAppOptions are several enum values OR'ed together.
Q_DECLARE_FLAGS( YQPkgAppOptions, YQPkgAppOption )
Q_DECLARE_OPERATORS_FOR_FLAGS( YQPkgAppOptions )

/**
 * Application class for yqpkg.
 **/
class MyrApp: public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * 'optFlags' are flags OR'ed together.
     **/
    MyrApp( YQPkgAppOptions optFlags = OptNone );

    /**
     * Destructor
     **/
    virtual ~MyrApp();

    /**
     * Return the instance of this class or 0 if there is none.
     *
     * This is not a real singleton, but for the lifetime of this application
     * this instance will remain alive, i.e. for most other classes related to
     * this.
     **/
    static MyrApp * instance() { return _instance; }

    /**
     * Run the application. This also handles the Qt event loop.
     **/
    void run();

    /**
     * Return 'true' if this program is running with root privileges
     * or if OptFakeRoot is set.
     *
     * To check the real privileges, use "geteuid() == 0" instead.
     **/
    static bool runningAsRoot();

    /**
     * Return 'true' if this program is running in read-only mode.
     **/
    static bool readOnlyMode() { return isOptionSet( OptReadOnly ); }

    /**
     * Return the whole option flags.
     **/
    static YQPkgAppOptions optFlags() { return _optFlags; }

    /**
     * Return 'true' if option 'opt' is set.
     **/
    static bool isOptionSet( YQPkgAppOption opt )
        { return _optFlags.testFlag( opt ); }

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
    YQPkgRepoManager *  _yqPkgRepoManager;
    ZyppLogger *        _zyppLogger;
    PkgTasks *          _pkgTasks;

    static MyrApp *   _instance;
    static YQPkgAppOptions      _optFlags;
};

#endif // MyrApp_h
