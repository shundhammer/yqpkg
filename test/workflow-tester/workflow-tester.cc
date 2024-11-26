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


#include <unistd.h>     // sleep()
#include <QString>

#include "../../src/Workflow.h"
#include "../../src/Logger.h"
#include "../../src/Exception.h"


class TestWfStep: public WorkflowStep
{
public:
    TestWfStep( const QString & name, int id, int next = -1 )
        : WorkflowStep( id, next )
        , _name( name )
        {}

    void activate  ( bool goingForward ) override;
    void deactivate( bool goingForward ) override;

protected:

    QString _name;
};


void TestWfStep::activate( bool goingForward )
{
    logDebug() << "  +++ " << _name << endl;
}


void TestWfStep::deactivate( bool goingForward )
{
    logDebug() << "--- " << _name << endl;
}



int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/yqpkg-$USER", "workflow-tester.log" );

    QList<WorkflowStep *> workflowSteps;
    workflowSteps << new TestWfStep( "First Step ID 2",  2 )
                  << new TestWfStep( "Step ID  4",       4, 8 ) // next: ID 8
                  << new TestWfStep( "Left out ID 7",    7 )    // Should almost never be seen
                  << new TestWfStep( "Step ID  8",       8 )
                  << new TestWfStep( "Step ID 16",      16 )
                  << new TestWfStep( "Last Step ID 32", 32 );

    Workflow * workflow = new Workflow( workflowSteps );


    logDebug() << "Going through the workflow past the end" << endl;

    for ( int i=0; i < workflowSteps.size() + 2; i++ )
    {
        workflow->next();
    }

    sleep( 1 );
    logNewline();
    logDebug() << "Going back 3 steps" << endl;

    for ( int i=0; i < 3; i++ )
    {
        workflow->back();
    }

    sleep( 1 );
    logNewline();
    logDebug() << "Going to step 16 directly" << endl;

    workflow->gotoStep( 16 );

    sleep( 1 );
    logNewline();
    logDebug() << "Restarting the workflow" << endl;

    workflow->restart();

    logDebug() << "The history should be empty now: "
               << QString( workflow->historyEmpty() ? "empty" : "not empty" )
               << endl;

    sleep( 1 );
    logNewline();
    logDebug() << "Trying to go back 2 times" << endl;
    workflow->back();
    workflow->back();

    logDebug() << "Current step ID: " << workflow->currentStep()->id() << endl;

    sleep( 1 );
    logNewline();
    logDebug() << "Going forward until the end" << endl;

    // for ( int i=0; i < workflowSteps.size(); i++ )
    while ( ! workflow->atLastStep() )
    {
        logDebug() << "Next" << endl;

        workflow->next();
    }


    sleep( 1 );
    logNewline();
    logDebug() << "Going backward until the start of the history" << endl;

    while ( ! workflow->historyEmpty() )
    {
        logDebug() << "Back" << endl;

        workflow->back();
    }


    sleep( 1 );
    logNewline();
    logDebug() << "Going on a wild ride" << endl;

    workflow->gotoStep(  7 );
    workflow->gotoStep( 16 );
    workflow->gotoStep(  4 );
    workflow->gotoStep( 32 );
    workflow->gotoStep( 2 );


    sleep( 1 );
    logNewline();
    logDebug() << "Going backward until the start of the history" << endl;

    while ( ! workflow->historyEmpty() )
    {
        workflow->back();
    }

    delete workflow;

    return 0;
}
