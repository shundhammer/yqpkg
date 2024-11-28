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

}


YQPkgAppWorkflowStep::~YQPkgAppWorkflowStep()
{
    if ( _doDeletePage && _page )
        delete _page;
}


void YQPkgAppWorkflowStep::activate( bool goingForward )
{
    Q_UNUSED( goingForward );

    if ( ! _page )
    {
        createPage();
        _app->mainWin()->addPage( _page );
    }

    _app->mainWin()->showPage( _page );

    if ( _doProcessEvents )
        _app->mainWin()->processEvents();
}


//----------------------------------------------------------------------


void YQPkgInitReposStep::activate( bool goingForward )
{
    YQPkgAppWorkflowStep::activate( goingForward ); // Show the page
    initRepos();  // Do the one-time work

    // Automatically continue with the next step in the same direction

    if ( goingForward || _app->workflow()->historyEmpty() )
        _app->next();
    else
        _app->back();
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

    YQPkgRepoManager * repoMan = _app->repoMan();
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


QWidget * YQPkgWizardStep::createPage()
{
    WizardPage * wizardPage = new WizardPage( _id );
    CHECK_NEW( wizardPage );

    wizardPage->connect( _app );

    return wizardPage;
}
