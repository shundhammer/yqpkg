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


#include <unistd.h>     // getuid()

#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "PkgCommitPage.h"
#include "PkgTasks.h"
#include "Workflow.h"
#include "SummaryPage.h"
#include "YQPkgSelector.h"
#include "YQi18n.h"
#include "YQPkgRepoManager.h"
#include "YQPkgAppWorkflowSteps.h"
#include "YQPkgApplication.h"
#include "ZyppLogger.h"


YQPkgApplication * YQPkgApplication::_instance = 0;
bool               YQPkgApplication::_fakeRoot = false;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _mainWin(0)
    , _workflow(0)
    , _pkgSel(0)
    , _pkgCommitPage(0)
    , _summaryPage(0)
    , _yqPkgRepoManager(0)
    , _zyppLogger(0)
    , _pkgTasks(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    if ( getenv( "YQPKG_FAKE_ROOT" ) )
    {
        logInfo() << "Faking root with environment variable YQPKG_FAKE_ROOT" << endl;
        _fakeRoot = true;
    }

    createMainWin(); // Create this early to get early visual feedback
    logDebug() << "Creating YQPkgApplication done" << endl;
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
        delete _pkgSel;

    if ( _pkgCommitPage )
        delete _pkgCommitPage;

    if ( _summaryPage )
        delete _summaryPage;

    if ( _workflow )
        delete _workflow;

    if ( _mainWin )
        delete _mainWin;

    if ( _yqPkgRepoManager )
        delete _yqPkgRepoManager;

    if ( _zyppLogger )
        delete _zyppLogger;

    _instance = 0;

    logDebug() << "Destroying YQPkgApplication done" << endl;
}


void YQPkgApplication::run()
{
    logDebug() << endl;

    if ( ! _workflow )
        createWorkflow();
    else
        _workflow->restart();

    // Control between main window pages and workflow steps is now entirely up
    // to the workflow object and the events passed back and forth between the
    // pages / workflow steps (each step has one page):
    //
    // They send signals to trigger slots like 'next()', 'back()', 'quit()',
    // 'restart()' (which goes back to the package selection).

    qApp->exec();  // Qt main event loop

    // Only 'quit()' or an exception terminate the event loop.
}


void YQPkgApplication::createMainWin()
{
    logDebug() << endl;

    if ( _mainWin )
        return;

    logDebug() << "Creating the main window" << endl;

    _mainWin = new MainWindow();
    CHECK_NEW( _mainWin );

    setWindowTitle( _mainWin );
    _mainWin->installEventFilter( this );
    _mainWin->show();
}


void YQPkgApplication::createWorkflow()
{
    if ( _workflow )
        return;

    logDebug() << "Creating the application workflow" << endl;

    WorkflowStepList steps;
    steps << new YQPkgInitReposStep( this, "initRepos" ) // excluded from history
          << new YQPkgSelStep      ( this, "pkgSel"    )
          << new YQPkgCommitStep   ( this, "pkgCommit" ) // excluded from history
          << new YQPkgSummaryStep  ( this, "summary"   );

    _workflow = new Workflow( steps );
    CHECK_PTR( _workflow );

    logDebug() << "Starting workflow" << endl;
    _workflow->start();
}


void YQPkgApplication::setWindowTitle( QWidget * window )
{
    if ( window )
    {
        QString windowTitle( "YQPkg" );
        windowTitle += runningAsRoot() ? _( " [root]" ) : _( " (read-only)" );
        window->setWindowTitle( windowTitle );
    }
}


YQPkgSelector *
YQPkgApplication::pkgSel()
{
    if ( ! _pkgSel )
        createPkgSel(); // Created lazy because this takes a moment

    return _pkgSel;
}

void YQPkgApplication::createPkgSel()
{
    if ( _pkgSel )
        return;

    QLabel busyPage( _( "Preparing the package selector..." ) );

    if ( _mainWin )
        _mainWin->splashPage( &busyPage );

    _pkgSel = new YQPkgSelector( 0, 0 );
    CHECK_NEW( _pkgSel );

    QObject::connect( _pkgSel, SIGNAL( commit() ),
                      this,    SLOT  ( next()   ) );

    QObject::connect( _pkgSel, SIGNAL( finished()   ),   // "accept" without changes
                      this,    SLOT  ( skipCommit() ) );
}


PkgCommitPage *
YQPkgApplication::pkgCommitPage()
{
    if ( ! _pkgCommitPage )
        createPkgCommitPage();

    return _pkgCommitPage;
}


void YQPkgApplication::createPkgCommitPage()
{
    if ( _pkgCommitPage )
        return;

    _pkgCommitPage = new PkgCommitPage();
    CHECK_NEW( _pkgCommitPage );

    QObject::connect( _pkgCommitPage, SIGNAL( next() ),
                      this,          SLOT  ( next() ) );
}


SummaryPage *
YQPkgApplication::summaryPage()
{
    if ( ! _summaryPage )
        createSummaryPage();

    return _summaryPage;
}


void YQPkgApplication::createSummaryPage()
{
    if ( _summaryPage )
        return;

    _summaryPage = new SummaryPage();
    CHECK_NEW( _summaryPage );

    QObject::connect( _summaryPage, SIGNAL( back() ),
                      this,         SLOT  ( back() ) );

    QObject::connect( _summaryPage, SIGNAL( finish() ),
                      this,         SLOT  ( finish() ) );

}


YQPkgRepoManager *
YQPkgApplication::repoManager()
{
    if ( ! _yqPkgRepoManager )
        createRepoManager();

    return _yqPkgRepoManager;
}


void YQPkgApplication::createRepoManager()
{
    if ( _yqPkgRepoManager )
        return;

    _yqPkgRepoManager = new YQPkgRepoManager();
    CHECK_NEW( _yqPkgRepoManager );
}


ZyppLogger *
YQPkgApplication::zyppLogger()
{
    if ( ! _zyppLogger )
        createZyppLogger();

    return _zyppLogger;
}


void YQPkgApplication::createZyppLogger()
{
    if ( _zyppLogger )
        return;

    _zyppLogger = new ZyppLogger();
    CHECK_NEW( _zyppLogger );
}


PkgTasks *
YQPkgApplication::pkgTasks()
{
    if ( ! _pkgTasks )
        createPkgTasks();

    return _pkgTasks;
}


void YQPkgApplication::createPkgTasks()
{
    if ( _pkgTasks )
        return;

    _pkgTasks = new PkgTasks();
    CHECK_NEW( _pkgTasks );
}


bool YQPkgApplication::runningAsRoot()
{
    if ( _fakeRoot )
        return true;

    return geteuid() == 0;
}


/**
 * Catch some events from other QObjects to redirect them if needed:
 * For example WM_CLOSE triggers the same as "Cancel" in the package selector.
 **/
bool YQPkgApplication::eventFilter( QObject * watchedObj, QEvent * event )
{
    if ( _mainWin && watchedObj == _mainWin && _pkgSel
	 && event && event->type() == QEvent::Close ) // WM_CLOSE (Alt-F4)
    {
        QWidget * currentPage = _mainWin->currentPage();

        if ( currentPage == _pkgSel )
        {
            logInfo() << "Caught WM_CLOSE for YQPkgSelector" << endl;
            _pkgSel->wmClose();  // _pkgSel handles asking for confirmation etc.

            return true;        // Event processing finished for this one
        }
        else if ( currentPage == _pkgCommitPage )
        {
            logInfo() << "Caught WM_CLOSE for PkgCommitPage" << endl;
            _pkgCommitPage->wmClose();

            return true;        // Event processing finished for this one
        }
        else
        {
            logInfo() << "Caught WM_CLOSE, but not for YQPkgSelector" << endl;
        }
    }

    return false; // Event processing not yet finished, continue down the chain
}


void YQPkgApplication::next()
{
    CHECK_PTR( _workflow );

    if ( ! _workflow )
        return;

    logDebug() << "Current page: " << _workflow->currentStep()->id() << endl;

    if ( _workflow->currentStep()->id() == "pkgCommit" )
    {
        // FIXME: Move this to the summary page once it's no longer a wizard placeholder page.

        if ( ! _pkgCommitPage->showSummaryPage() )
        {
            logInfo() << "Skipping summary page -> quitting" << endl;
            quit();

            return;
        }
    }

    if ( _workflow->atLastStep() )
    {
        logDebug() << "This was the last step." << endl;
        finish();
    }
    else
    {
        logDebug() << "Next step in the workflow." << endl;
        _workflow->next();
    }
}


void YQPkgApplication::finish()
{
    logDebug() << "Quitting the program." << endl;
    quit();
}


void YQPkgApplication::back()
{
    logDebug() << endl;
    CHECK_PTR( _workflow );

    if ( ! _workflow )
        return;

    _workflow->back();
}


void YQPkgApplication::restart()
{
    logDebug() << endl;
    CHECK_PTR( _workflow );

    if ( ! _workflow )
        return;

    _workflow->gotoStep( "pkgSel" );
}


void YQPkgApplication::skipCommit()
{
    // The user finished the package selection with "Accept", but there was no
    // change: Skip the "commit" phase and go straight to the summary screen.

    logDebug() << endl;
    CHECK_PTR( _workflow );

    if ( ! _workflow )
        return;

    _workflow->gotoStep( "summary" );
}


void YQPkgApplication::quit( bool askForConfirmation )
{
    logDebug() << endl;

    if ( askForConfirmation )
    {
	int result = QMessageBox::warning( _mainWin,
                                           "",  // Window title
                                           _( "Really quit?" ),
                                           _( "&Yes" ), _( "&No" ), "",
                                           0,   // defaultButtonNumber (from 0)
                                           1 ); // escapeButtonNumber
        if ( result != 0 ) // button #0 (Yes)
            return;
    }

    if ( _mainWin )
        _mainWin->hide();  // For instant user feedback

    qApp->quit();
}
