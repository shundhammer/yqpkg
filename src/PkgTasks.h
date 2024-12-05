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


#ifndef PkgTasks_h
#define PkgTasks_h


#include <QString>
#include <QList>


/**
 * Types of actions for a packate task.
 *
 * Notice the absence of static package states like "don't install", "keep",
 * "taboo", "protected": Those are not tasks; there is nothing to do for them.
 **/
enum PkgTaskAction
{
    PkgNoAction    = 0,

    PkgInstall     = 0x01,
    PkgUpdate      = 0x02,
    PkgRemove      = 0x04,

    // Some OR'ed values

    PkgAdd         = 0x03, // PkgInstall | PkgInstall
    PkgAll         = 0x07  // PkgInstall | PkgInstall | PkgRemove
};


/**
 * Who requested the package task
 **/
enum PkgTaskRequester
{
    PkgReqNone = 0,

    PkgReqUser = 0x01, //
    PkgReqDep  = 0x02, // Dependencies

    PkgReqAll  = 0x03 // PkgReqUser | PkgReqDep
};


/**
 * Class representing one package task with a package name, what action
 * (install / update / remove) and who requested it: Was this requested
 * explicitly by the user, or automatically (by the dependency resolver) to
 * satisfy dependencies.
 *
 * Notice that this class intentionally does not keep track of the package
 * version or architecture, or if the task is already done, still to do, or if
 * there were errors.
 **/
class PkgTask
{
public:

    /**
     * Constructor.
     **/
    PkgTask( const QString &  pkgName,
             PkgTaskAction    pkgAction,
             PkgTaskRequester requester ) // PkgReqUser or PkgReqDep
        : _name( pkgName )
        , _action( pkgAction )
        , _requester( requester )
        {}

    /**
     * Return the package name.
     **/
    const QString & name() const { return _name; }

    /**
     * Return the action that is to do or done: Install, update, remove.
     **/
    PkgTaskAction action() const { return _action; }

    /**
     * Return who requested the task: The user (PkgReqUser) or dependencies
     * (the dependency resolver): PkgReqDep.
     **/
    PkgTaskRequester requester() const { return _requester; }

    /**
     * Return 'true' if this action was requested by the user and not
     * automatically to satisfy dependencies.
     **/
    bool byUser() const { return _requester | PkgReqUser; }

    /**
     * Return 'true' if this action was requested to satisfy dependencies and
     * not explicitly by the user.
     *
     * Notice that this can't use a function named 'auto()' since 'auto' is a
     * reserved word in C++. Too bad.
     **/
    bool byDependency() const { return _requester | PkgReqDep; }

    /**
     * Comparison operator, needed by some QList operations like 'indexOf()' or
     * 'contains()'.
     **/
    bool operator==( const PkgTask & other );


protected:

    QString          _name;
    PkgTaskAction    _action;
    PkgTaskRequester _requester;
};


/**
 * A list of package tasks.
 *
 * Notice that the tasks are stored in the list directly and not as pointers.
 **/
class PkgTaskList: public QList<PkgTask>
{
public:

    PkgTaskList()
        : QList<PkgTask>()
        {}

    /**
     * Add a task to this list unless that same task is already in the list
     * (with the same package name, action and requester).
     **/
    void add( const PkgTask & task );

    // To find a task, use indexOf( const PkgTask & ).
    // To take it out of the list, use indexOf() and then takeAt( index ).

    /**
     * Return a new list from origList filtered by action and requester.
     *
     * Remember that both PkgTaskAction and PkgTaskRequester support OR'ed values.
     **/
    PkgTaskList filtered( PkgTaskAction    action,
                          PkgTaskRequester requester = PkgReqAll ) const;

};



/**
 * Class representing package tasks as lists according to their status: to do,
 * doing, done, failed.
 *
 * This is just a very thin layer around the package task lists; the getters
 * for the lists all return a non-const reference so the application can use
 * package list (and QList) functions directly.
 *
 * Some convenience functions are provided.
 **/
class PkgTasks
{
public:
    /**
     * Constructor.
     **/
    PkgTasks() {}

    /**
     * Initialize the lists from zypp: Clear all lists, then iterate over all
     * selectables and add those with a relevant status to the 'to do' list.
     **/
    void initFromZypp();

    /**
     * Return the list of package tasks that are to do (not done yet, pending).
     **/
    PkgTaskList & todo()   { return _todo; }

    /**
     * Return the list of package tasks that are currently being done.
     **/
    PkgTaskList & doing()  { return _doing; }

    /**
     * Return the list of package tasks that are successfully done.
     **/
    PkgTaskList & done()   { return _doing; }

    /**
     * Return the list of package tasks that failed.
     **/
    PkgTaskList & failed() { return _failed; }

    /**
     * Move a task from one list to another.
     **/
    void moveTask( const PkgTask & task,
                   PkgTaskList &   fromList,
                   PkgTaskList &   toList );

    /**
     * Clear all lists.
     **/
    void clearAll();

protected:

    PkgTaskList _todo;
    PkgTaskList _doing;
    PkgTaskList _done;
    PkgTaskList _failed;
};

#endif // PkgTasks_h
