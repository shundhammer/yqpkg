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


#include <unistd.h>             // usleep()

#include <QApplication>
#include <QSettings>
#include <QMessageBox>

#include "Logger.h"
#include "Exception.h"
#include "YQi18n.h"
#include "PkgCommitPage.h"


PkgCommitPage * PkgCommitPage::_instance = 0;


PkgCommitPage::PkgCommitPage( QWidget * parent )
    : QWidget( parent )
    , _ui( new Ui::PkgCommitPage ) // Use the Qt designer .ui form (XML)
    , _showingDetails( false )
{
    CHECK_PTR( _ui );
    _ui->setupUi( this ); // Actually create the widgets from the .ui form

    // See ui_pkg-commit-page.h in ../build/yqkg_autogen/include for the
    // widgets' variable names. They are taken from the widget names in the .ui
    // form (an XML file) that was generated with Qt designer, so choose them
    // carefully when using Qt designer.

    readSettings();
    reset();
    connectWidgets();

    _instance = this;
}


PkgCommitPage::~PkgCommitPage()
{
    writeSettings();
    delete _ui;
    _instance = 0;
}


void PkgCommitPage::connectWidgets()
{

    connect( _ui->detailsButton, SIGNAL( clicked()       ),
             this,               SLOT  ( toggleDetails() ) );

    connect( _ui->cancelButton,  SIGNAL( clicked() ),
             this,               SLOT  ( cancelCommit()  ) );
}

void PkgCommitPage::commit()
{
    logInfo() << "Starting package transactions" << endl;

    // DEBUG
    // DEBUG
    // DEBUG

    for ( int i=1; i <= 100; ++i )
    {
        usleep( 100 * 1000 ); // microseconds
        _ui->totalProgressBar->setValue( i );
        processEvents();
    }

    // DEBUG
    // DEBUG
    // DEBUG

    logInfo() << "Package transactions done" << endl;
}


bool PkgCommitPage::showSummaryPage() const
{
    return _ui->showSummaryPageCheckBox->isChecked();
}


void PkgCommitPage::reset()
{
    _ui->totalProgressBar->setValue( 0 );

    _ui->toDoList->clear();
    _ui->doingList->clear();
    _ui->doneList->clear();

    _ui->detailsFrame->setVisible( _showingDetails );
    updateDetailsButton();
}


void PkgCommitPage::toggleDetails()
{
    _showingDetails = ! _showingDetails;
    _ui->detailsFrame->setVisible( _showingDetails );
    updateDetailsButton();
}


void PkgCommitPage::updateDetailsButton()
{
    QString text = showingDetails() ?
        _( "Hide &Details" ) : _( "Show &Details" );

    _ui->detailsButton->setText( text );
}


void PkgCommitPage::cancelCommit()
{
    if ( askForCancelCommitConfirmation() )
        emit next();
}


void PkgCommitPage::wmClose()
{
    if ( askForCancelCommitConfirmation() )
        qApp->quit();
}


bool PkgCommitPage::askForCancelCommitConfirmation()
{
    // Not all users might know what "package transactions" means,
    // so let's be a bit clearer (albeit less precise)

    QString msg = _( "Really cancel the current package actions?" );

    int result = QMessageBox::warning( this, // parent widget
                                       "",   // Window title
                                       msg,
                                       _( "&Yes" ), _( "&No" ), "",
                                       0,   // defaultButtonNumber (from 0)
                                       1 ); // escapeButtonNumber
    return result == 0;  // button #0 (Yes)
}


void PkgCommitPage::readSettings()
{
    QSettings settings;
    settings.beginGroup( "PkgCommitPage" );

    _showingDetails      = settings.value( "showingDetails",  false ).toBool();
    bool showSummaryPage = settings.value( "showSummaryPage", true ).toBool();

    settings.endGroup();

    _ui->showSummaryPageCheckBox->setChecked( showSummaryPage );
}


void PkgCommitPage::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "PkgCommitPage" );

    settings.setValue( "showingDetails",  _showingDetails );
    settings.setValue( "showSummaryPage", showSummaryPage() );

    settings.endGroup();
}


void PkgCommitPage::processEvents()
{
    // This is just an alias for now, but it might change in the future.
    QCoreApplication::processEvents();
}

