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


#ifndef ProgressDialog_h
#define ProgressDialog_h

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>


// Generated with 'uic' from a Qt designer .ui form: progress-dialog.ui
//
// Check out ../build/src/yqpkg_autogen/include/ui_progress-dialog-page.h
// for the variable names of the widgets.

#include "ui_progress-dialog.h"

/**
 * Simple progress bar dialog with a label above and a "Close" button below the
 * progress bar. The "Close" button is already set up to close the dialog.
 *
 * This is very much like the standard QProgressDialog, but with a better visual
 * appearance.
 **/
class ProgressDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * If 'parent' is 0, it will use MainWindow::instance(), so the dialog is
     * centered above the main window.
     **/
    ProgressDialog( const QString & text, QWidget * parent = 0 );

    /**
     * Destructor.
     **/
    virtual ~ProgressDialog();

    /**
     * Reset internal values. In particular, it sets the progress bar value to
     * 0.
     **/
    void reset();

    /**
     * Return the current progressbar value in percent.
     **/
    int value() const { return progressBar()->value(); }

    /**
     * Set the progressbar value in percent (0..100).
     **/
    void setValue( int val ) { progressBar()->setValue( val ); }

    /**
     * Set the text of the label above the progress bar.
     **/
    void setText( const QString & text ) { label()->setText( text ); }


    // Access to the widgets

    QLabel *       label()       const { return _ui->label;       }
    QProgressBar * progressBar() const { return _ui->progressBar; }
    QPushButton *  closeButton() const { return _ui->closeButton; }

    //
    // Data members
    //

    Ui::ProgressDialog * _ui;
};

#endif // ProgressDialog_h
