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


#ifndef YQPkgSelectorBase_h
#define YQPkgSelectorBase_h

#include <QFrame>

#include "YQZypp.h"


class QY2ComboTabWidget;
class QAction;
class QCloseEvent;
class QKeyEvent;

class YQPkgConflictDialog;
class YQPkgDiskUsageList;
class YQPkgSelWmCloseHandler;


#define YPkg_TestMode		1 << 0	// Test mode for debugging
#define YPkg_OnlineUpdateMode	1 << 1	// Online update mode: Show patches
#define YPkg_UpdateMode		1 << 2	// Start with "Update problems" filter view
#define YPkg_SearchMode		1 << 3	// Start with "Search"  filter view
#define YPkg_SummaryMode	1 << 4	// Start with "Summary" filter view
#define YPkg_RepoMode		1 << 5	// Start with "Repositories" filter view
#define YPkg_RepoMgr		1 << 6	// Add "Manage Repositories" to menu
#define YPkg_ConfirmUnsupported	1 << 7	// Confirm unsupported packages


/**
 * Abstract base class for package selectors.
 **/
class YQPkgSelectorBase: public QFrame
{
    Q_OBJECT

protected:

    /**
     * Constructor
     *
     * Will initialize package and selection managers and create conflict
     * dialogs.
     **/
    YQPkgSelectorBase( QWidget * parent, long modeFlags = 0 );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelectorBase();


public slots:

    /**
     * Reset all content in connected views.
     * See also signal 'resetNotify()'.
     **/
    void reset();

    /**
     * Resolve dependencies (unconditionally) for all resolvables.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int resolveDependencies();

    /**
     * Verifies dependencies of the currently installed system.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int verifySystem();

    /**
     * Check for disk overflow and post a warning dialog if necessary.
     * The user can choose to override this warning.
     *
     * Returns QDialog::Accepted if no warning is necessary or if the user
     * wishes to override the warning, QDialog::Rejected otherwise.
     **/
    int checkDiskUsage();

    /**
     * Display a list of automatically selected packages
     * (excluding packages contained in any selections that are to be installed)
     **/
    void showAutoPkgList();

    /**
     * Close processing and abandon changes.
     * If there were changes, this will post an "Abandon all changes?" pop-up.
     **/
    void reject();

    /**
     * WM_CLOSE: ALT-F4 or the close window [x] icon in the window decoration.
     **/
    void wmClose() { reject(); }

    /**
     * Close processing and accept changes
     **/
    void accept();

    /**
     * Return 'true' if there are any pending changes,
     * i.e. package transactions that need to be committed.
     **/
    bool pendingChanges();

    /**
     * Inform user about a feature that is not implemented yet.
     * This should NEVER show up in the final version.
     **/
    void notImplemented();


signals:

    /**
     * Notification to reset all content, for example because we just went back
     * in the workflow after a package commit, and now the new states of the
     * packages need to be displayed; what was 'install' before the package
     * commit is now 'installed', which is different. Connected views should
     * use this to reload their content.
     **/
    void resetNotify();

    /**
     * Emitted when package resolving is started.
     * This can be used for some kind of "busy display".
     **/
    void resolvingStarted();

    /**
     * Emitted when package resolving is finished.
     **/
    void resolvingFinished();

    /**
     * Emitted when the user accepted and solved all pending dependency
     * problems: Commit the package transactions.
     **/
    void commit();

    /**
     * Emitted when the user accepted, and there is nothing to be done:
     * No package actions to commit.
     **/
    void finished();


protected slots:

    /**
     * Reset all previously ignored dependency problems.
     **/
    void resetIgnoredDependencyProblems();


protected:

    //
    // Checks for the various modes
    //

    bool testMode()		const { return _modeFlags & YPkg_TestMode;	   	}
    bool onlineUpdateMode()	const { return _modeFlags & YPkg_OnlineUpdateMode; 	}
    bool updateMode()		const { return _modeFlags & YPkg_UpdateMode;	   	}
    bool searchMode() 		const { return _modeFlags & YPkg_SearchMode;	   	}
    bool summaryMode()		const { return _modeFlags & YPkg_SummaryMode;	   	}
    bool repoMode()		const { return _modeFlags & YPkg_RepoMode;         	}
    bool repoMgrEnabled()	const { return _modeFlags & YPkg_RepoMgr;		}
    bool confirmUnsupported()	const { return _modeFlags & YPkg_ConfirmUnsupported;	}

    /**
     * Show all license agreements the user has not confirmed yet
     * (for all packages that will be installed, and in YOU mode also for
     * patches).
     *
     * Returns 'true' if all licenses were confirmed, 'false' if one or more
     * licenses were not confirmed (in which case some packages might be set to
     * S_TABOO, which might require another resolver run).
     **/
    bool showPendingLicenseAgreements();

    /**
     * Show all license agreements in a resolvable range. To be used with
     * zyppPkgBegin() and zyppPkgEnd() or with zyppPatchesBegin() and
     * zyppPatchesEnd().
     **/
    bool showPendingLicenseAgreements( ZyppPoolIterator begin,
				       ZyppPoolIterator end );

    /**
     * Event handler for keyboard input - for debugging and testing.
     *
     * Reimplemented from QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * event );

    /**
     * Event handler for WM_CLOSE (Alt-F4).
     *
     * Reimplemented from QWidget.
     **/
    virtual void closeEvent( QCloseEvent * event );


    // Data members

    long                        _modeFlags;
    bool                        _blockResolver;
    bool			_showChangesDialog;
    YQPkgConflictDialog *	_pkgConflictDialog;
    YQPkgDiskUsageList *	_diskUsageList;
    QAction *			_actionResetIgnoredDependencyProblems;
};



#endif // YQPkgSelectorBase_h
