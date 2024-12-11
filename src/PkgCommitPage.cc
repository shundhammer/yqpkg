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

#include "Exception.h"
#include "Logger.h"
#include "PkgTasks.h"
#include "PkgTaskListWidget.h"
#include "YQPkgApplication.h"
#include "YQZypp.h"
#include "YQi18n.h"
#include "utf8.h"
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
    _totalDownloadSize      = 0.0;
    _totalInstalledSize     = 0.0;
    _totalTasksCount        = pkgTasks()->todo().size();

    _completedDownloadSize  = 0.0;
    _completedInstalledSize = 0.0;
    _completedTasksCount    = 0;

    foreach ( PkgTask task, pkgTasks()->todo() )
    {
        if ( ( task.action() & PkgAdd ) && task.downloadSize() > 0.0 )
            _totalDownloadSize += task.downloadSize();

        if ( task.installedSize() > 0.0 )
            _totalInstalledSize += task.installedSize();
    }

    logDebug() << "total download size: "  << _totalDownloadSize  << endl;
    logDebug() << "total installed size: " << _totalInstalledSize << endl;
    logDebug() << "total tasks: "          << _totalTasksCount    << endl;

    // Weights for different sub-tasks of downloading and installing packages:
    // There is a constant cost for doing anything with a package, no matter if
    // it's installing or removing it: The 'handling' of the package.
    //
    // Of course a large part of the cost is the download, and another is the
    // cost of actually installing or removing it, be it unpacking an RPM (for
    // installing a package) or removing it (removing its file list entries).

    _pkgDownloadWeight      = 0.55;
    _pkgInstallRemoveWeight = 0.30;
    _pkgFixedCostWeight     = 0.15;
}


float PkgCommitPage::doingDownloadSizeSum()
{
    float sum = 0.0;

    foreach ( PkgTask task, pkgTasks()->doing() )
    {
        if ( ( task.action() & PkgAdd )    &&
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


int PkgCommitPage::currentProgressPercent()
{
    float downloadPercent  = 0.0;
    float installedPercent = 0.0;
    float tasksPercent     = 0.0;
    float usedWeight       = 0.0;

    if ( _totalDownloadSize > 0 ) // Prevent division by zero
    {
        float downloadSize = _completedDownloadSize  + doingDownloadSizeSum();
        float percent      = 100.0 * downloadSize / _totalDownloadSize;
        downloadPercent    = percent * _pkgDownloadWeight;
        usedWeight        += _pkgDownloadWeight;

        logDebug() << "DL %:   "  << downloadPercent
                   << " weight: " << _pkgDownloadWeight
                   << " raw %: "  << percent
                   << endl;
    }

    if ( _totalInstalledSize > 0 )
    {
        float installedSize = _completedInstalledSize + doingInstalledSizeSum();
        float percent       = 100.0 * installedSize / _totalInstalledSize;
        installedPercent    = percent * _pkgInstallRemoveWeight;
        usedWeight         += _pkgInstallRemoveWeight;

        logDebug() << "Inst %: "  << installedPercent
                   << " weight: " << _pkgInstallRemoveWeight
                   << " raw %: "  << percent
                   << endl;
    }

    if ( _totalTasksCount > 0 )
    {
        float percent       = 100.0 * _completedTasksCount / (float) _totalTasksCount;
        float tasksPercent  = percent * _pkgFixedCostWeight;
        usedWeight         += _pkgFixedCostWeight;

        logDebug() << "Task %: "  << tasksPercent
                   << " weight: " << _pkgFixedCostWeight
                   << " raw %: "  << percent
                   << endl;
    }

    if ( qFuzzyCompare( usedWeight, (float) 0.0 ) )
        return 0;

    float progress   = tasksPercent + downloadPercent + installedPercent;
    logDebug() << "Initial progress: " << progress << endl;

    float totalWeight = _pkgFixedCostWeight + _pkgDownloadWeight + _pkgInstallRemoveWeight;
    progress *= totalWeight / usedWeight;

    logDebug() << "Final progress: "  << progress
               << " total weight: "   << totalWeight
               << " used weight: "    << usedWeight
               << " scaling factor: " << totalWeight / usedWeight
               << endl;

    return qBound( 0, (int) ( progress + 0.5 ), 100 );
}


bool PkgCommitPage::updateTotalProgressBar()
{
    bool didUpdate   = false;
    int  oldProgress = _ui->totalProgressBar->value();
    int  progress    = currentProgressPercent();

    if ( progress >= 0 && oldProgress != progress )
    {
        logDebug() << "Updating with " << progress << "%" << endl;
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

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;


    // Locate the task in the todo list

    int taskIndex = pkgTasks()->todo().indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in todo" << endl;
        return;
    }

    // Move the task from the todo list to the doing list

    PkgTask task = pkgTasks()->todo().takeAt( taskIndex );
    task.setDownloadedPercent( 0 ); // Just to make sure
    pkgTasks()->doing().add( task );

    // Move the task from the todo list widget to the doing list widget

    _ui->todoList->removeTaskItem( task );
    _ui->doingList->addTaskItem( task );

    processEvents(); // Update the UI
}


void PkgCommitPage::pkgDownloadProgress( ZyppRes zyppRes, int percent )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << ": " << percent << "%" << endl;


    // Locate the task in the doing list

    PkgTaskList & taskList = pkgTasks()->doing();
    int taskIndex = taskList.indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in doing" << endl;
        return;
    }

    PkgTask & task = taskList[ taskIndex ];

    if ( percent != task.downloadedPercent() )
    {
        task.setDownloadedPercent( percent );

        // Update the UI

        if ( updateTotalProgressBar() ) // This is a bit expensive
            processEvents();
    }
}


void PkgCommitPage::pkgDownloadEnd( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;


    // Locate the task in the doing list

    PkgTaskList & taskList = pkgTasks()->doing();
    int taskIndex = taskList.indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << endl;
        return;
    }

    PkgTask & task = taskList[ taskIndex ];
    task.setDownloadedPercent( 100 ); // Just making sure

    // Important: Not adding the download size to _completedDownloadSize just
    // yet, or it would be counted twice while the task is still in the doing
    // list. That has to wait until it is moved from the doing list to the done
    // list.
}


void PkgCommitPage::pkgInstallStart( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;

    // Check if the task is already in the doing list

    int taskIndex = pkgTasks()->doing().indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 ) // No, it's not yet in doing - move it there
    {
        taskIndex = pkgTasks()->todo().indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

        if ( taskIndex < 0 )
        {
            logError() << "Can't find task for " << name
                       << " in either doing or todo" << endl;
            return;
        }

        // Move the task from the todo list to the doing list

        PkgTask task = pkgTasks()->todo().takeAt( taskIndex );
        task.setCompletedPercent( 0 ); // Just to make sure
        pkgTasks()->doing().add( task );

        // Move the task from the todo list widget to the doing list widget

        _ui->todoList->removeTaskItem( task );
        _ui->doingList->addTaskItem( task );

        // Update the UI

        processEvents();


        // No
        //
        //  _completedDownloadSize += task.downloadSize()
        //
        // here while the task is in the doing list, otherwise it would be
        // summed up twice!
    }
    else // The task already was in doing (no download needed)
    {
        PkgTask & task = pkgTasks()->doing()[ taskIndex ];
        task.setDownloadedPercent( 100 ); // The download is complete for sure
        task.setCompletedPercent( 0 );  // But the task itself isn't completed

        // Not sure if this warrants a forced UI update with processEvents()

        // No
        //
        //  _completedDownloadSize += task.downloadSize()
        //
        // here while the task is in the doing list, otherwise it would be
        // summed up twice!
    }
}


void PkgCommitPage::pkgInstallProgress( ZyppRes zyppRes, int percent )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << ": " << percent << "%" << endl;


    // Locate the task in the doing list

    PkgTaskList & taskList = pkgTasks()->doing();
    int taskIndex = taskList.indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in doing" << endl;
        return;
    }

    PkgTask & task = taskList[ taskIndex ];

    if ( percent != task.completedPercent() )
    {
        task.setCompletedPercent( percent );

        // Update the UI

        if ( updateTotalProgressBar() ) // This is a bit expensive
            processEvents();
    }
}


void PkgCommitPage::pkgInstallEnd ( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;

    // Locate the task in the doing list

    int taskIndex = pkgTasks()->doing().indexOf( PkgTask( name, PkgAdd, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in doing" << endl;
        return;
    }

    // Move the task from the doing list to the done list

    PkgTask task = pkgTasks()->doing().takeAt( taskIndex );
    task.setDownloadedPercent( 100 );
    task.setCompletedPercent( 100 ); // Just to make sure
    pkgTasks()->done().add( task );

    // Move the task from the doing list widget to the done list widget

    _ui->doingList->removeTaskItem( task );
    _ui->doneList->addTaskItem( task );


    // Update the internal bookkeeping sums

    ++_completedTasksCount;

    if ( task.installedSize() > 0 )
        _completedInstalledSize += task.installedSize();

    if ( task.downloadSize() > 0 )
        _completedDownloadSize += task.downloadSize();


    // Update the UI

    updateTotalProgressBar(); // This may or may not be needed
    processEvents();          // But this is needed for sure
}


void PkgCommitPage::pkgRemoveStart( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;


    // Locate the task in the todo list

    int taskIndex = pkgTasks()->todo().indexOf( PkgTask( name, PkgRemove, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in todo" << endl;
        return;
    }

    // Move the task from the todo list to the doing list

    PkgTask task = pkgTasks()->todo().takeAt( taskIndex );
    task.setCompletedPercent( 0 ); // Just to make sure
    pkgTasks()->doing().add( task );

    // Move the task from the todo list widget to the doing list widget

    _ui->todoList->removeTaskItem( task );
    _ui->doingList->addTaskItem( task );

    // Update the UI

    processEvents();
}


void PkgCommitPage::pkgRemoveProgress( ZyppRes zyppRes, int percent )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << ": " << percent << "%" << endl;


    // Locate the task in the doing list

    PkgTaskList & taskList = pkgTasks()->doing();
    int taskIndex = taskList.indexOf( PkgTask( name, PkgRemove, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in doing" << endl;
        return;
    }

    PkgTask & task = taskList[ taskIndex ];

    if ( percent != task.completedPercent() )
    {
        task.setCompletedPercent( percent );

        // Update the UI

        if ( updateTotalProgressBar() ) // This is a bit expensive
            processEvents();
    }
}


void PkgCommitPage::pkgRemoveEnd( ZyppRes zyppRes )
{
    CHECK_PTR( zyppRes );

    QString name = fromUTF8( zyppRes->name() );
    logInfo() << name << endl;

    // Locate the task in the doing list

    int taskIndex = pkgTasks()->doing().indexOf( PkgTask( name, PkgRemove, PkgReqAll ) );

    if ( taskIndex < 0 )
    {
        logError() << "Can't find task for " << name << " in doing" << endl;
        return;
    }

    // Move the task from the doing list to the done list

    PkgTask task = pkgTasks()->doing().takeAt( taskIndex );
    task.setCompletedPercent( 100 ); // Just to make sure
    pkgTasks()->done().add( task );

    // Move the task from the doing list widget to the done list widget

    _ui->doingList->removeTaskItem( task );
    _ui->doneList->addTaskItem( task );

    // Update the internal bookkeeping sums

    ++_completedTasksCount;

    if ( task.installedSize() > 0 )
        _completedInstalledSize += task.installedSize();

    // Update the UI

    updateTotalProgressBar(); // This may or may not be needed
    processEvents();          // But this is needed for sure
}


//----------------------------------------------------------------------
