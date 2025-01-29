/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef RepoConfigDialog_h
#define RepoConfigDialog_h

#include <QDialog>


// Generated with 'uic' from a Qt designer .ui form: repo-config.ui
//
// Check out ../build/src/myrlyn_autogen/include/ui_repo-config.h
// for the variable names of the widgets.

#include "ui_repo-config.h"


/**
 * Dialog to configure repos with a RepoTable at the top, some widgets for the
 * current repo below that, and a row of buttons at the bottom.
 *
 * This dialog uses "instant apply", i.e. any changes are active immediately,
 * and just a "Close" button to dismiss it.
 **/
class RepoConfigDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    RepoConfigDialog( QWidget * parent = 0 );

    /**
     * Destructor
     **/
    virtual ~RepoConfigDialog();


signals:

    /**
     * Emitted when the user changed any details of the current repo like
     * priority, enabled status, autorefresh status.
     **/
    void currentStatusChanged();


protected slots:

    /**
     * Notification from the RepoTable that the currently selected item changed.
     **/
    void currentChanged();

    /**
     * Notification from the "priority" spinbox, the "enabled" checkbox or the
     * "autorefresh" checkbox that one of the details changed.
     **/
    void currentEdited();

protected:

    /**
     * Set up signal / slot connections.
     **/
    void connectWidgets();

    /**
     * Update the data for the current repo in the widgets below the table.
     **/
    void updateCurrentData();


    // Data members

    Ui::RepoConfig * _ui;  // see ui_repo-config.h
};


#endif // RepoConfigDialog_h
