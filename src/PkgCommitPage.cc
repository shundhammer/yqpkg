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
#include <stdlib.h>             // abs()

#include <QSettings>
#include <QMessageBox>

#include "Logger.h"
#include "Exception.h"
#include "PkgTasks.h"
#include "PkgTaskListWidget.h"
#include "YQPkgApplication.h"
#include "YQi18n.h"
#include "YQZypp.h"
#include "PkgCommitCallbacks.h"
#include "PkgCommitPage.h"


PkgCommitPage * PkgCommitPage::_instance = 0;


PkgCommitPage::PkgCommitPage( QWidget * parent )
    : QWidget( parent )
    , _ui( new Ui::PkgCommitPage ) // Use the Qt designer .ui form (XML)
    , _pkgTasks( 0 )
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
    PkgCommitSignalForwarder::instance()->connectAll( this );

    _instance = this;
}


PkgCommitPage::~PkgCommitPage()
{
    writeSettings();
    delete _ui;
    // PkgCommitSignalForwarder::instance()->deleteLater();

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
    populateLists();
    initProgressCosts();
    _ui->totalProgressBar->setValue( 0 );

    if ( YQPkgApplication::isOptionSet( OptFakeCommit ) )
        fakeCommit();
    else
        realCommit();
}


void PkgCommitPage::populateLists()
{
    _ui->todoList->clear();
    _ui->doingList->clear();
    _ui->doneList->clear();

    _ui->todoList->addTaskItems( pkgTasks()->todo() );

    // The other lists are almost certainly empty anyway at this point, but
    // let's make sure, not make assumptions; this will be over very quickly if
    // they are actually empty.

    _ui->doingList->addTaskItems( pkgTasks()->doing() );
    _ui->doneList->addTaskItems ( pkgTasks()->done()  );
}


PkgTasks * PkgCommitPage::pkgTasks()
{
    if ( ! _pkgTasks )
        _pkgTasks =  YQPkgApplication::instance()->pkgTasks();

    CHECK_PTR( _pkgTasks );

    return _pkgTasks;
}


void PkgCommitPage::fakeCommit()
{
    logInfo() << "Simulating package transactions" << endl;

    for ( int i=1; i <= 100; ++i )
    {
        usleep( 100 * 1000 ); // microseconds
        _ui->totalProgressBar->setValue( i );
        processEvents();
    }

    logInfo() << "Simulating transactions done" << endl;
}


void PkgCommitPage::realCommit()
{
    logInfo() << "Starting package transactions" << endl;
    processEvents();

    // Create and install the callbacks.
    // They are uninstalled when the 'callbacks' variable goes out of scope.
    PkgCommitCallbacks callbacks;

    zypp::getZYpp()->commit( commitPolicy() );

    logInfo() << "Package transactions done" << endl;
}


zypp::ZYppCommitPolicy
PkgCommitPage::commitPolicy() const
{
    zypp::ZYppCommitPolicy policy;

    if ( YQPkgApplication::isOptionSet( OptDryRun ) )
    {
        logInfo() << "dry run" << endl;
        policy.dryRun( true );
    }

    if ( YQPkgApplication::isOptionSet( OptDownloadOnly ) )
    {
        logInfo() << "download only" << endl;
        policy.downloadMode( zypp::DownloadOnly );
    }

    return policy;
}


bool PkgCommitPage::showSummaryPage() const
{
    return _ui->showSummaryPageCheckBox->isChecked();
}


void PkgCommitPage::reset()
{
    _ui->totalProgressBar->setValue( 0 );

    _ui->todoList->clear();
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
        YQPkgApplication::instance()->quit();
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
    QCoreApplication::processEvents( QEventLoop::AllEvents,
                                     500 ); //millisec
}


void PkgCommitPage::initProgressCosts()
{
    int   totalTasksCount        = pkgTasks()->todo().size();
    float totalDownloadSize      = 0.0;
    float totalInstalledSize     = 0.0;

    _completedTasksCount    = 0;
    _completedDownloadSize  = 0.0;
    _completedInstalledSize = 0.0;

    foreach ( PkgTask task, pkgTasks()->todo() )
    {
        if ( ( task.action() | PkgAdd ) && task.downloadSize() > 0.0 )
            totalDownloadSize += task.downloadSize();

        if ( task.installedSize() > 0.0 )
            totalInstalledSize += task.installedSize();
    }

    // Weights for different sub-tasks of downloading and installing packages:
    // There is a constant cost for doing anything with a package, no matter if
    // it's installing or removing it: The 'handling' of the package.
    //
    // Of course a large part of the cost is the download, and another is the
    // cost of actually installing or removing it, be it unpacking an RPM (for
    // installing a package) or removing it (removing its file list entries).

    _pkgFixedCostWeight     = 0.2;
    _pkgDownloadWeight      = 0.6;
    _pkgInstallRemoveWeight = 0.4;

    _totalTasksCost =
        totalTasksCount    * _pkgFixedCostWeight     +
        totalDownloadSize  * _pkgDownloadWeight      +
        totalInstalledSize * _pkgInstallRemoveWeight;

    logDebug() << "Total tasks cost: " << _totalTasksCost << endl;
}


float PkgCommitPage::doingDownloadSizeSum()
{
    float sum = 0.0;

    foreach ( PkgTask task, pkgTasks()->doing() )
    {
        if ( ( task.action() | PkgAdd )    &&
             task.downloadSize()      > 0  &&
             task.downloadedPercent() > 0 )
        {
            sum += task.downloadSize() * ( task.downloadedPercent() / 100.0 );
        }
    }

    return sum;
}


float PkgCommitPage::doingInstalledSizeSum()
{
    float sum = 0.0;

    foreach ( PkgTask task, pkgTasks()->doing() )
    {
        if ( task.installedSize() > 0 && task.completedPercent() > 0 )
            sum += task.installedSize() * ( task.completedPercent() / 100.0 );
    }

    return sum;
}


float PkgCommitPage::currentTasksCost()
{
    // Intentionally not taking the fixed cost for tasks in 'doing' into
    // account since they are not completed yet.

    float cost = _completedTasksCount * _pkgFixedCostWeight +
        ( _completedDownloadSize  + doingDownloadSizeSum()  ) * _pkgDownloadWeight +
        ( _completedInstalledSize + doingInstalledSizeSum() ) * _pkgInstallRemoveWeight;

    return cost;
}


int PkgCommitPage::currentProgressPercent()
{
    if ( qFuzzyCompare( (double) _totalTasksCost, 0.0 ) )
    {
        logError() << "_totalTasksCost is 0; preventing division by 0" << endl;
        return -1;
    }

    float progress = currentTasksCost() / _totalTasksCost * 100.0;

    return qBound( 0, (int) ( progress + 0.5 ), 100 );
}


bool PkgCommitPage::updateTotalProgressBar()
{
    bool didUpdate   = false;
    int  oldProgress = _ui->totalProgressBar->value();
    int  progress    = currentProgressPercent();

    if ( progress >= 0 && oldProgress != progress )
    {
        _ui->totalProgressBar->setValue( progress );
        didUpdate = true;
    }

    return didUpdate;
}


//----------------------------------------------------------------------

//
// PkgCommitCallback slots
//

void PkgCommitPage::pkgDownloadStart( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


void PkgCommitPage::pkgDownloadProgress( ZyppRes zyppRes, int value )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << ": " << value << "%" << endl;
    _ui->totalProgressBar->setValue( value );
    processEvents();
}


void PkgCommitPage::pkgDownloadEnd( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


void PkgCommitPage::pkgInstallStart( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


void PkgCommitPage::pkgInstallProgress( ZyppRes zyppRes, int value )
{
    CHECK_PTR( zyppRes );
    // logInfo() << zyppRes->name() << ": " << value << "%" << endl;

    if ( value <= 3 || value >= 98 || ( value % 5  == 0 ) )
    {
        logInfo() << zyppRes->name() << ": " << value << "%" << endl;
        _ui->totalProgressBar->setValue( value );
        processEvents();
    }
}


void PkgCommitPage::pkgInstallEnd ( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


void PkgCommitPage::pkgRemoveStart( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


void PkgCommitPage::pkgRemoveProgress( ZyppRes zyppRes, int value )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << ": " << value << "%" << endl;
    _ui->totalProgressBar->setValue( value );
    processEvents();
}


void PkgCommitPage::pkgRemoveEnd( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    logInfo() << zyppRes->name() << endl;
}


//----------------------------------------------------------------------
