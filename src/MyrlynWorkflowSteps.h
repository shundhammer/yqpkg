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


#ifndef MyrlynWorkflowSteps_h
#define MyrlynWorkflowSteps_h


#include "Workflow.h"

class MyrlynApp;


/**
 * Base class for all workflow steps in the Myrlyn app
 **/
class MyrlynWorkflowStep: public WorkflowStep
{
public:

    MyrlynWorkflowStep( MyrlynApp * app,
                          const QString &    id,
                          const QString &    next );

    virtual ~MyrlynWorkflowStep();

    /**
     * Default implementation for activating this workflow step:
     * Show the MainWindow page for this step. Create it if it doesn't exist
     * yet.
     **/
    virtual void activate( bool goingForward ) override;

protected:

    /**
     * Get the MainWindow page for this step.
     * Ownership remains with the derived class; it won't be deleted.
     *
     * Derived classes are required to implement either this method or
     * createPage().
     **/
    virtual QWidget * page() { return 0; }

    /**
     * Create a MainWindow page for this step.
     *
     * Ownership is transferred to this class; it will be deleted in the
     * Workflow destructor (unless the _doDeletePage flag is set to 'false' in
     * the reimplemented createPage() method).
     *
     * Derived classes are required to implement either this method or
     * page().
     **/
    virtual QWidget * createPage() { return 0; };

    /**
     * Go to the next page in the same direction.
     *
     * If there is no more page in the history while going back, change
     * direction to forward and go to the next page.
     **/
    void nextPage( bool goingForward );

    /**
     * Get the page for this step, either with page() or with createPage(),
     * and add it to the MainWindow pages.
     *
     * This will throw if none of them is implemented and returns a page.
     **/
    void ensurePage();

protected:


    //
    // Data members
    //

    MyrlynApp *  _app;
    QPointer<QWidget>   _page;
    bool                _doProcessEvents;
    bool                _doDeletePage;
};


/**
 * The "init repos" step
 **/
class YQPkgInitReposStep: public MyrlynWorkflowStep
{
public:

     YQPkgInitReposStep( MyrlynApp * app,
                         const QString &    id,
                         const QString &    next = QString() )
         : MyrlynWorkflowStep( app, id, next )
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
class YQPkgSelStep: public MyrlynWorkflowStep
{
public:

    YQPkgSelStep( MyrlynApp * app,
                  const QString &    id,
                  const QString &    next = QString() )
        : MyrlynWorkflowStep( app, id, next )
        {}

protected:

    /**
     * Get the page for this step: The package selector.
     *
     * Since this subclass implements page() and not createPage(),
     * ownership of the page is not transferred to the Workflow,
     * so it is not deleted.
     **/
    virtual QWidget * page() override;

    /**
     * Reset the package selector when needed.
     **/
    virtual void activate( bool goingForward ) override;
};


/**
 * The "package commit" step
 **/
class YQPkgCommitStep: public MyrlynWorkflowStep
{
public:

    YQPkgCommitStep( MyrlynApp * app,
                     const QString &    id,
                     const QString &    next = QString() )
        : MyrlynWorkflowStep( app, id, next )
        {
            setExcludeFromHistory();
        }


    virtual void activate( bool goingForward ) override;

protected:

    /**
     * Get the page for this step: The package committer.
     *
     * Since this subclass implements page() and not createPage(),
     * ownership of the page is not transferred to the Workflow,
     * so it is not deleted.
     **/
    virtual QWidget * page() override;
};


class YQPkgSummaryStep: public MyrlynWorkflowStep
{
public:

    YQPkgSummaryStep( MyrlynApp * app,
                      const QString &    id,
                      const QString &    next = QString() )
        : MyrlynWorkflowStep( app, id, next )
        {}

protected:

    /**
     * Create the page for this step: The summary page.
     *
     * Since this subclass implements page() and not createPage(),
     * ownership of the page is not transferred to the Workflow,
     * so it is not deleted.
     **/
    virtual QWidget * page() override;

    virtual void activate  ( bool goingForward ) override;
    virtual void deactivate( bool goingForward ) override;
};


#if 0
/**
 * A generic wizard step with "Back" and "Next" buttons
 **/
class YQPkgWizardStep: public MyrlynWorkflowStep
{
public:

    YQPkgWizardStep( MyrlynApp * app,
                     const QString &    id,
                     const QString &    next = QString() )
        : MyrlynWorkflowStep( app, id, next )
        {}

protected:

    /**
     * Create the page for this step: A wizard page.
     *
     * Since this subclass implements createPage and not page(),
     * ownwership of the page is transferred to the Workflow,
     * and it is deleted in the Workflow destructor.
     **/
    virtual QWidget * createPage() override;

    virtual void activate( bool goingForward ) override;
};
#endif


#endif // MyrlynWorkflowSteps_h
