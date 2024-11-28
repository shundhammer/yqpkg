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


#include <QWidget>
#include "ui_wizard-page.h"    // Generated with 'uic' from a Qt designer .ui form


#ifndef WizardPage_h
#define WizardPage_h


/**
 * Simple wizard page with a label and a button box with "Back" and "Next".
 *
 * On this level, this is useful as a placeholder for wizard workflow pages
 * that are still in development.
 **/
class WizardPage: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * 'name' is used to be displayed in the page content.
     **/
    WizardPage( const QString & name, QWidget * parent = 0 );

    /**
     * Destructor
     **/
    ~WizardPage();

    /**
     * Connect the "Back" and "Next" signals of this class to slots named
     * "back()" and "next()" of a receiver QObject.
     *
     * If the slots are named differently, use a normal QObject::connect with
     * the signal names from here and the real slot names for the receiver.
     **/
    void connect( QObject * receiver );


signals:

    /**
     * Emitted when the user clicks the "Back" button or activates the "Back"
     * action.
     **/
    void back();

    /**
     * Emitted when the user clicks the "Next" button or activates the "Next"
     * action.
     **/
    void next();


protected:

    Ui::WizardPage * _ui;       // See ui_wizard_page.h
};


#endif // WizardPage_h
