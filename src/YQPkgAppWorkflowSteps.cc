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
#include "PkgCommitter.h"
#include "WizardPage.h"
#include "YQPackageSelector.h"
#include "YQPkgRepoManager.h"
#include "YQi18n.h"
#include "YQPkgApplication.h"
#include "YQPkgAppWorkflowSteps.h"


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


void YQPkgAppWorkflowStep::activate( bool goingForward )
{
    Q_UNUSED( goingForward );
    logDebug() << "Activating step " << _id << endl;

    if ( ! _page )
    {
        _page = createPage();
        CHECK_PTR( _page );

        _app->mainWin()->addPage( _page );
    }

    CHECK_PTR( _page );
    _app->mainWin()->showPage( _page );

    if ( _doProcessEvents )
        _app->mainWin()->processEvents();
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


QWidget *
YQPkgInitReposStep::createPage()
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
        repoMan->initTarget();
        repoMan->attachRepos();
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

    logDebug() << "Initializing zypp done" << endl;
    _reposInitialized = true;
}


//----------------------------------------------------------------------


QWidget *
YQPkgSelStep::createPage()
{
    _doDeletePage = false; // We don't own it, the app object does

    return _app->pkgSel();
}


//----------------------------------------------------------------------


QWidget *
YQPkgCommitStep::createPage()
{
    _doDeletePage    = false; // We don't own it, the app object does
    _doProcessEvents = true;  // Make sure to display this page

    return _app->pkgCommitter();
}



void YQPkgCommitStep::activate( bool goingForward )
{
    if ( goingForward )
    {
        YQPkgAppWorkflowStep::activate( goingForward ); // Show the page
        _app->pkgCommitter()->reset();  // Reset the widgets on the page

        _app->pkgCommitter()->commit(); // Do the package transactions
    }

    nextPage( goingForward );
}


//----------------------------------------------------------------------


QWidget * YQPkgWizardStep::createPage()
{
    WizardPage * wizardPage = new WizardPage( _id );
    CHECK_NEW( wizardPage );

    wizardPage->connect( _app );

    return wizardPage;
}
