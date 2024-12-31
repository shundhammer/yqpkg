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


#include <QMessageBox>

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "PkgCommitPage.h"
#include "SummaryPage.h"
#include "YQPkgSelector.h"
#include "YQPkgRepoManager.h"
#include "YQi18n.h"
#include "YQPkgApplication.h"
#include "YQPkgAppWorkflowSteps.h"

#if 0
#include "WizardPage.h"
// If needed, move the source files back here  from the attic/ subdirectory:
//
//   WizardPage.h
//   WizardPage.cc
//   wizard-page.ui
//
// and re-enable them in CMakeLists.txt
#endif


YQPkgAppWorkflowStep::YQPkgAppWorkflowStep( YQPkgApplication * app,
                                            const QString &    id,
                                            const QString &    next )
    : WorkflowStep( id, next )
    , _app( app )
    , _page(0)
    , _doProcessEvents( false )
    , _doDeletePage( true )
{
    logDebug() << "Creating step " << _id << endl;
}


YQPkgAppWorkflowStep::~YQPkgAppWorkflowStep()
{
    logDebug() << "Destroying step " << _id << "..." << endl;

    if ( _doDeletePage && _page )
    {
        logDebug() << "  Deleting page of step " << _id << endl;
        delete _page;
    }

    logDebug() << "Destroying step " << _id << " done" << endl;
}


void YQPkgAppWorkflowStep::ensurePage()
{
    if ( _page )
        return;

    _page = page(); // First try: Get it without creating

    if ( _page )
        _doDeletePage = false; // We don't own this one
    else
    {
        // Second try: Create it

        _doDeletePage = true;  // We own it now (by default)
        _page = createPage();
    }

    if ( ! _page )      // Still no page?
    {
        logError() << "FATAL: Implement one of page() or createPage()" << endl;
        CHECK_PTR( _page );
    }

    _app->mainWin()->addPage( _page );
}


void YQPkgAppWorkflowStep::activate( bool goingForward )
{
    Q_UNUSED( goingForward );
    logDebug() << "Activating step " << _id << endl;

    ensurePage();
    _app->mainWin()->showPage( _page );

    if ( _doProcessEvents )
        MainWindow::processEvents();
}


void YQPkgAppWorkflowStep::nextPage( bool goingForward )
{
    // Automatically continue with the next step in the same direction

    if ( goingForward || _app->workflow()->historyEmpty() )
    {
        logDebug() << "_app->next()" << endl;
        _app->next();
    }
    else
    {
        logDebug() << "_app->back()" << endl;
        _app->back();
    }
}


//----------------------------------------------------------------------


void YQPkgInitReposStep::activate( bool goingForward )
{
    YQPkgAppWorkflowStep::activate( goingForward ); // Show the page

    if ( goingForward )
        initRepos();  // Do the one-time work

    nextPage( goingForward );
}


QWidget * YQPkgInitReposStep::createPage()
{
    _doProcessEvents = true; // This is a splash screen, so process events

    return new QLabel( _( "Loading package manager data..." ) );
}


void YQPkgInitReposStep::initRepos()
{
    if ( _reposInitialized )
        return;

    logDebug() << "Initializing zypp..." << endl;

    YQPkgRepoManager * repoMan = _app->repoManager();
    CHECK_PTR( repoMan );

    try
    {
        repoMan->zyppConnect(); // This may throw
    }
    catch ( ... )
    {
        QString message = _( "Can't connect to the package manager!\n"
                             "It may be busy in another window.\n" );

        QMessageBox::warning( 0, // parent
                              _( "Error" ),
                              message,
                              QMessageBox::Ok );
        throw;  // Nothing else that we can do here
    }

    repoMan->initTarget();
    repoMan->attachRepos();

    logDebug() << "Initializing zypp done" << endl;
    _reposInitialized = true;
}


//----------------------------------------------------------------------


QWidget *
YQPkgSelStep::page()
{
    return _app->pkgSel();
}


void YQPkgSelStep::activate( bool goingForward )
{
    YQPkgAppWorkflowStep::activate( goingForward ); // Show the page

    // Reset not only if 'goingForward' is false: We get here from the summary
    // page with a 'gotoStep( "pkgSel")' call directly, and in that case
    // 'goingForward' is true.

    _app->pkgSel()->reset();
}


//----------------------------------------------------------------------


QWidget * YQPkgCommitStep::page()
{
    _doProcessEvents = true;  // Make sure to display this page

    return _app->pkgCommitPage();
}


void YQPkgCommitStep::activate( bool goingForward )
{
    if ( goingForward )
    {
        YQPkgAppWorkflowStep::activate( goingForward ); // Show the page
        _app->pkgCommitPage()->reset();  // Reset the widgets on the page

        _app->pkgCommitPage()->commit(); // Do the package transactions
    }

    nextPage( goingForward );
}


//----------------------------------------------------------------------


QWidget * YQPkgSummaryStep::page()
{
    return _app->summaryPage();
}


void YQPkgSummaryStep::activate( bool goingForward )
{
    YQPkgAppWorkflowStep::activate( goingForward );

    _app->summaryPage()->updateSummary();
    _app->summaryPage()->startCountdown();
}


void YQPkgSummaryStep::deactivate( bool goingForward )
{
    _app->summaryPage()->stopCountdown();
    YQPkgAppWorkflowStep::deactivate( goingForward );
}


//----------------------------------------------------------------------


#if 0

QWidget * YQPkgWizardStep::createPage()
{
    WizardPage * wizardPage = new WizardPage( _id );
    CHECK_NEW( wizardPage );

    wizardPage->connect( _app );

    return wizardPage;
}

#endif
