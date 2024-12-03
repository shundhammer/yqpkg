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


#ifndef SummaryPage_h
#define SummaryPage_h

#include <QWidget>
#include <QTimer>


// Generated with 'uic' from a Qt designer .ui form: summary-page.ui
//
// Check out ../build/src/yqpkg_autogen/include/ui_summary-page.h
// for the variable names of the widgets.

#include "ui_summary-page.h"


class QAction;
class QActionGroup;


/**
 * Summary page with buttons "Back" and "Finish" and a countdown.
 **/
class SummaryPage: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    SummaryPage( QWidget * parent = 0 );

    /**
     * Destructor
     **/
    ~SummaryPage();

    /**
     * Reset the summary to defaults.
     **/
    void reset();

    /**
     * Fill the summary text content.
     **/
    void updateSummary();

    /**
     * Return the countdown duration in seconds or 0 if there is no countdown.
     *
     * This is the maximum value when the timer is started / restarted, not the
     * currently remaining seconds.
     **/
    int countdown() const { return _countdownSec; }

    /**
     * Set the countdown in seconds. 0 will disable the timer.
     * This does NOT start or stop the timers.
     **/
    void setCountdown( int seconds ) { _countdownSec = seconds; }


public slots:

    /**
     * Start the countdown timers according to the current value of
     * countdown(). If that value is 0, no timer will be started.
     **/
    void startCountdown();

    /**
     * Stop the countdown timers.
     **/
    void stopCountdown();


signals:

    /**
     * Emitted when the user clicks the "Back" button.
     **/
    void back();

    /**
     * Emitted when the user clicks the "Finish" button.
     **/
    void finish();


protected slots:

    /**
     * Close this page when the countdown reached zero.
     **/
    void timeout();

    /**
     * Update and enable / disable / hide / show the widgets related to the
     * countdown.
     **/
    void updateCountdownWidgets();

    /**
     * Configure the countdown value from a QAction.
     * The new value is taken from data() of the sending action.
     **/
    void configureCountdown( QAction * action );


protected:

    /**
     * Set up the "..." countdown configuration menu button.
     **/
    void setupCountdownMenuButton();

    /**
     * Add a menu action to the "..." countdown configuration menu.
     *
     * The text is generated automatically from the 'seconds' parameter unless
     * 'overrideText' is non-empty.
     **/
    void addMenuAction( QActionGroup *  actionGroup,
                        int             seconds,
                        const QString & overrideText = QString() );

    /**
     * Set up the internal signal / slot connections.
     **/
    void connectWidgets();

    /**
     * Read and apply the settings from the config file.
     **/
    void readSettings();

    /**
     * Write the settings to the config file.
     **/
    void writeSettings();


    //
    // Data members
    //

    Ui::SummaryPage *   _ui;       // See ui_summary-page.h
    int                 _countdownSec;
    QTimer              _countdownTimer;
    QTimer              _intervalTimer;
    QMenu *             _countdownMenu;
};


#endif // SummaryPage_h
