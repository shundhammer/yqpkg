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


#ifndef YQPkgAppWorkflowSteps_h
#define YQPkgAppWorkflowSteps_h


#include "Workflow.h"

class YQPkgApplication;


/**
 * Base class for all workflow steps in an YQPkg app
 **/
class YQPkgAppWorkflowStep: public WorkflowStep
{
public:

    YQPkgAppWorkflowStep( YQPkgApplication * app,
                          const QString &    id,
                          const QString &    next );

    virtual ~YQPkgAppWorkflowStep();

    /**
     * Default implementation for activating this workflow step:
     * Show the MainWindow page for this step. Create it if it doesn't exist
     * yet.
     **/
    virtual void activate( bool goingForward ) override;

protected:

    /**
     * Create a MainWindow page for this step.
     *
     * Derived classes are required to implement this.
     **/
    virtual QWidget * createPage() = 0;

    //
    // Data members
    //

    YQPkgApplication *  _app;
    QWidget *           _page;
    bool                _doProcessEvents;
    bool                _doDeletePage;
};


/**
 * The "init repos" step
 **/
class YQPkgInitReposStep: public YQPkgAppWorkflowStep
{
public:

     YQPkgInitReposStep( YQPkgApplication * app,
                         const QString &    id,
                         const QString &    next = QString() )
         : YQPkgAppWorkflowStep( app, id, next )
         , _reposInitialized( false )
        {
            setExcludeFromHistory();
        }

    /**
     * Create and show the page for this step (a busy page)
     * and do the one-time work that is the reason for this step,
     * then automatically continue to the next step
     * (or to the previous step if we are going backward).
     **/
    virtual void activate( bool goingForward ) override;

protected:

    /**
     * Create the page for this step: A busy page.
     **/
    virtual QWidget * createPage() override;

    /**
     * Do the one-time work for this step:
     *
     * Initialize and attach the repos:
     *
     *   - Create the YQPkgRepoManager
     *   - Connect to libzypp
     *   - initialize the target (load the resolvables from the RPMDB)
     *   - attach all active repos
     **/
    void initRepos();

    // Data members

    bool _reposInitialized;
};


/**
 * The "package selection" step
 **/
class YQPkgSelStep: public YQPkgAppWorkflowStep
{
public:

    YQPkgSelStep( YQPkgApplication * app,
                  const QString &    id,
                  const QString &    next = QString() )
        : YQPkgAppWorkflowStep( app, id, next )
        {}

protected:

    /**
     * Create the page for this step: The package selector.
     **/
    virtual QWidget * createPage() override;
};


/**
 * A generic wizard step with "Back" and "Next" buttons
 **/
class YQPkgWizardStep: public YQPkgAppWorkflowStep
{
public:

    YQPkgWizardStep( YQPkgApplication * app,
                     const QString &    id,
                     const QString &    next = QString() )
        : YQPkgAppWorkflowStep( app, id, next )
        {}

protected:

    /**
     * Create the page for this step: A wizard page.
     **/
    virtual QWidget * createPage() override;
};


#endif // YQPkgAppWorkflowSteps_h
