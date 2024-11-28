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
#include "Workflow.h"
#include "YQPackageSelector.h"
#include "YQi18n.h"
#include "YQPkgRepoManager.h"
#include "YQPkgAppWorkflowSteps.h"
#include "YQPkgApplication.h"


YQPkgApplication * YQPkgApplication::_instance = 0;
bool               YQPkgApplication::_fakeRoot = false;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _mainWin(0)
    , _workflow(0)
    , _pkgSel(0)
    , _yqPkgRepoManager(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    if ( getenv( "YQPKG_FAKE_ROOT" ) )
    {
        logInfo() << "Faking root with environment variable YQPKG_FAKE_ROOT" << endl;
        _fakeRoot = true;
    }

    createMainWin(); // Create this early to get early visual feedback
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
        delete _pkgSel;

    if ( _mainWin )
        delete _mainWin;

    if ( _workflow )
        delete _workflow;

    if ( _yqPkgRepoManager )
        delete _yqPkgRepoManager;

    _instance = 0;

    logDebug() << "Destroying YQPkgApplication done" << endl;
}


void YQPkgApplication::run()
{
    if ( _workflow )
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

    // Only 'quit()' or an exception terminates the event loop
}


void YQPkgApplication::createMainWin()
{
    if ( _mainWin )
        return;

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
          << new YQPkgWizardStep   ( this, "pkgCommit" )
          << new YQPkgWizardStep   ( this, "summary"   );

    _workflow = new Workflow( steps );
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


YQPackageSelector *
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

    _pkgSel = new YQPackageSelector( 0, 0 );
    CHECK_PTR( _pkgSel );

    QObject::connect( _pkgSel, SIGNAL( commit() ),
                      this,    SLOT  ( next()   ) );
}


bool YQPkgApplication::runningAsRoot()
{
    if ( _fakeRoot )
        return true;

    return geteuid() == 0;
}


YQPkgRepoManager *
YQPkgApplication::repoMan()
{
    if ( ! _yqPkgRepoManager )
        createRepoMan();

    return _yqPkgRepoManager;
}


void YQPkgApplication::createRepoMan()
{
    if ( _yqPkgRepoManager )
        return;

    _yqPkgRepoManager = new YQPkgRepoManager();
    CHECK_NEW( _yqPkgRepoManager );
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
        if ( _mainWin->currentPage() == _pkgSel )
        {
            logInfo() << "Caught WM_CLOSE for YQPackageSelector" << endl;
            _pkgSel->reject();  // _pkgSel handles asking for confirmation etc.

            return true;        // Event processing finished for this one
        }
        else
        {
            logInfo() << "Caught WM_CLOSE, but not for YQPackageSelector" << endl;
        }
    }

    return false; // Event processing not yet finished, continue down the chain
}


void YQPkgApplication::next()
{
    if ( ! _workflow )
        return;

    if ( _workflow->atLastStep() )
        quit(); // TO DO: ask for confirmation?
    else
        _workflow->next();
}


void YQPkgApplication::back()
{
    if ( ! _workflow )
        return;

    _workflow->back();
}


void YQPkgApplication::restart()
{
    if ( ! _workflow )
        return;

    _workflow->gotoStep( "pkgSel" );
}


void YQPkgApplication::quit( bool askForConfirmation )
{
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

    qApp->quit();
}
