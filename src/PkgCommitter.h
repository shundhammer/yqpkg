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



#ifndef PkgCommitter_h
#define PkgCommitter_h

#include <QWidget>


// Generated with 'uic' from a Qt designer .ui form: pkg-commit.ui
//
// Check out ../build/src/yqpkg_autogen/include/ui_pkg-commit-page.h
// for the variable names of the widgets.

#include "ui_pkg-commit-page.h"


/**
 * Class for the "package commit" workflow step / page:
 *
 * Start the pending package transactions that have been collected in the
 * package selector; start installing / updating / removing packages.
 *
 * This is mostly a passive workflow step / page; it triggers the package
 * transactions in libzypp and displays the progress, mostly in a large
 * progress bar that visualizes the total progress of all the operations
 * combined, i.e. all the package downloads, installations, updates, removals,
 * and the pre- and post-installation (etc.) scripts.
 *
 * There is also one button that is active all the time to cancel / abort the
 * package transactions in emergency situations (when they are stuck or take a
 * very long time because of network problems).
 *
 * In a "detail" mode, this may also show more information about the packages
 * that are still to do and those that are done, and those that are in "doing",
 * i.e. that are currently actively being downloaded and installed / updated /
 * removed.
 *
 * libzypp may choose to do multiple operations in parallel, so more than one
 * package might be in "doing".
 *
 * This whole class relies heavily on libzypp callbacks reporting progress and
 * possible problems.
 **/
class PkgCommitter: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * This just creates the widget tree for the page; it doesn't start the
     * package transactions yet. Use commit() for that.
     **/
    PkgCommitter( QWidget * parent = 0 );

    /**
     * Destructor.
     **/
    virtual ~PkgCommitter();

    /**
     * Reset the widgets and the internal transaction status:
     * Set the progress back to 0 and clear any previous list content,
     **/
    void reset();

    /**
     * Start the package transactions.
     *
     * The passes control mostly to libzypp which has to call callbacks to
     * update the widgets reporting the progress. The callbacks will also do a
     * little Qt event processing (some milliseconds) to give the user a chance
     * to use the "Cancel" button or other interactive widgets.
     *
     * When this function returns (which will take a while), all the package
     * transactions should be done, or there was an unrecoverable error.
     **/
    void commit();

    /**
     * Return 'true' if the "Summary" page should be shown.
     **/
    bool showSummaryPage() const;

    /**
     * Return the UI (the widget tree) of this page.
     **/
    Ui::PkgCommitPage * ui() const { return _ui; }

    /**
     * Process the pending Qt events.
     *
     * This is necessary in callbacks to receive user events (e.g. button
     * clicks) and to update the display.
     *
     * DO NOT use MainWindow::processEvents() instead which might ignore user
     * input events (e.g. clicks on buttons).
     **/
    static void processEvents();

    /**
     * Return the instance of this class or 0 if there is none.
     *
     * This is not a singleton, but in each normal application there is no more
     * than one instance, and during the lifetime of any widgets inside the
     * MainWindow it is safe to assume that the MainWindow also exists.
     **/
    static PkgCommitter * instance() { return _instance; }

    /**
     * Return the UI (the widget tree) of the instance of this class or 0 if
     * there is none.
     *
     * This is mostly intended as a convenience for libzypp callbacks which
     * need to be static functions, so they don't have direct access to data
     * members (including widgets) of the instance of this class.
     **/
    static Ui::PkgCommitPage * uiInstance()
        { return _instance ? _instance->ui() : 0; }


public slots:

    /**
     * Cancel the package transactions.
     * This will ask for confirmation first.
     **/
    void cancelCommit();

    /**
     * WM_CLOSE: ALT-F4 or the close window [x] icon in the window decoration.
     * Cancel the package transactions and quit.
     * This will ask for confirmation first.
     **/
    void wmClose();


signals:

    /**
     * Emitted when the transactions are finished (successfully or with an
     * error), and the next workflow page should be displayed.
     *
     * Notice that there is intentionally no 'back()' counterpart.
     **/
    void next();


protected slots:

    /**
     * Switch displaying details on or off.
     **/
    void toggleDetails();


protected:

    /**
     * Set up the internal Qt signal / slot connections.
     **/
    void connectWidgets();

    /**
     * Ask the user for confirmation if the package transactions should really
     * be cancelled. Return 'true' if the user confirms, 'false' if not.
     **/
    bool askForCancelCommitConfirmation();

    /**
     * Return 'true' if details are currently shown, 'false' if not.
     **/
    bool showingDetails() const { return _showingDetails; }

    /**
     * Update the label of the details button between "Show Details" and "Hide
     * Details".
     **/
    void updateDetailsButton();

    /**
     * Read settings for this widget from the configuration file.
     **/
    void readSettings();

    /**
     * Write the settings of this widget to the configuration file.
     **/
    void writeSettings();

    //
    // Data members
    //

    Ui::PkgCommitPage *   _ui;
    bool                  _showingDetails;

    static PkgCommitter * _instance;
};


#endif  // PkgCommitter_h

