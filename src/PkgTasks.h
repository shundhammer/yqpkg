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
#include <QMutex>

#include "YQZypp.h"     // ZyppRes



/**
 * Types of actions for a packate task.
 *
 * Notice the absence of static package states like "don't install", "keep",
 * "taboo", "protected": Those are not tasks; there is nothing to do for them.
 **/
enum PkgTaskAction
{
    PkgNoAction = 0,

    PkgInstall  = 0x01,
    PkgUpdate   = 0x02,
    PkgRemove   = 0x04,

    // Some OR'ed values

    PkgAdd      = 0x03, // PkgInstall | PkgUpdate
    PkgAll      = 0x07  // PkgInstall | PkgUpdate | PkgRemove
};


/**
 * Who requested the package task
 **/
enum PkgTaskRequester
{
    PkgReqNone = 0,

    PkgReqUser = 0x01, //
    PkgReqDep  = 0x02, // Dependencies

    PkgReqAll  = 0x03  // PkgReqUser | PkgReqDep
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
 *
 * It can be used to store some numeric values like the download or the
 * installed size (in bytes) or the downloaded or completed percent, but those
 * fields are just for convenience during the package commit stage.
 *
 * The status of each task is implicit by what list it is in: todo, doing,
 * done, failed.
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
        , _downloadSize ( -1.0 )
        , _installedSize( -1.0 )
        , _downloadedPercent( -1 )
        , _completedPercent( -1 )
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
     * Return the download size in bytes or -1.0 (< 0.0) if unknown.
     * This is irrelevant for actions like PkgRemove.
     *
     * This uses 'float' for ease of handling and to avoid overflows. The value
     * does not need to be exact to the byte level, just a rough value to
     * calculate the total progress percent.
     **/
    float downloadSize() const { return _downloadSize; }

    /**
     * Set the download size in bytes.
     **/
    void setDownloadSize( float value ) { _downloadSize = value; }

    /**
     * Return the installed size in bytes or -1.0 (< 0.0) if unknown.
     *
     * This uses 'float' for ease of handling and to avoid overflows. The value
     * does not need to be exact to the byte level, just a rough value to
     * calculate the total progress percent.
     **/
    float installedSize() const { return _installedSize; }

    /**
     * Set the installed size in bytes.
     **/
    void setInstalledSize( float value ) { _installedSize = value; }

    /**
     * Return the downloaded percent (0..100) or -1 if unknown.
     * This is not relevant if this is a package remove action.
     **/
    int downloadedPercent() const { return _downloadedPercent; }

    /**
     * Set the downloaded percent (0..100).
     **/
    void setDownloadedPercent( int value ) { _downloadedPercent = value; }

    /**
     * Return percent (0..100) to which this task is completed or -1 if
     * unknown.
     **/
    int completedPercent() const { return _completedPercent; }

    /**
     * Set the completed percent (0..100).
     **/
    void setCompletedPercent( int value ) { _completedPercent = value; }

    /**
     * Return 'true' if this action matches the specified name, action and
     * requester. If 'name' is empyt, don't check the name, just action and
     * requester.
     **/
    bool matches( const QString &  name,
                  PkgTaskAction    action    = PkgAll,
                  PkgTaskRequester requester = PkgReqAll ) const;

    /**
     * Return 'true' if this action matches the other action's name, action and
     * requester. If the 'name' field of 'other' is empyt, don't check the
     * name, just action and requester.
     **/
    bool matches( PkgTask *       other ) const;
    bool matches( const PkgTask & other ) const;


protected:

    QString          _name;
    PkgTaskAction    _action;
    PkgTaskRequester _requester;

    float            _downloadSize;       // Bytes
    float            _installedSize;      // Bytes
    int              _downloadedPercent;  // 0..100 or -1 for unknown
    int              _completedPercent;   // 0..100 or -1 for unknown
};


/**
 * A list of package tasks.
 **/
class PkgTaskList: public QList<PkgTask *>
{
public:

    /**
     * Constructor.
     **/
    PkgTaskList( const QString & listName );

    /**
     * Destructor.
     **/
    virtual ~PkgTaskList();

    /**
     * Find the first action that matches the specified name, action and
     * requester. If 'name' is empty, only action and requester are checked.
     *
     * Return the task if found, 0 if not found.
     **/
    PkgTask * find( const QString &  name,
                    PkgTaskAction    action    = PkgAll,
                    PkgTaskRequester requester = PkgReqAll ) const;

    /**
     * Find the first action that matches the specified name, action and
     * requester of 'filter'. If 'filter.name()' is empty, only action and
     * requester are checked.
     *
     * Return the task if found, 0 if not found.
     **/
    PkgTask * find( const PkgTask & filter ) const;

    /**
     * Find the first task where the name matches the name of the 'zyppRes'
     * Zypp resolvable.

     * Return the task if found, 0 if not found.
     **/
    PkgTask * find( ZyppRes zyppRes ) const;

    /**
     * Return a new list from origList filtered by action and requester.
     * Ownership of the tasks remains with the original list - do not delete them!
     **/
    PkgTaskList filtered( PkgTaskAction    action,
                          PkgTaskRequester requester = PkgReqAll );

    /**
     * Return the list name ("todo", "doing", "done", "failed" for debugging.
     **/
    QString name() const { return _name; }


private:

#if 0
    // Disable copy ctor and assignment operator

    PkgTaskList( const PkgTaskList & );
    PkgTaskList operator=( const PkgTaskList & );
#endif

protected:

    QString _name;
};


/**
 * Class representing package tasks as lists according to their status: to do,
 * doing, done, failed.
 *
 * This is just a very thin layer around the package task lists; the getters
 * for the lists all return a non-const reference so the application can use
 * package list (and QList) functions directly.
 *
 * Some convenience methods are provided.
 **/
class PkgTasks
{
public:

    PkgTasks();
    virtual ~PkgTasks();

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
    PkgTaskList & done()   { return _done; }

    /**
     * Return the list of package tasks that failed.
     **/
    PkgTaskList & failed() { return _failed; }

    /**
     * Move a task from one list to another.
     **/
    static void moveTask( PkgTask *      task,
                          PkgTaskList &  fromList,
                          PkgTaskList &  toList );

    /**
     * Clear all lists, i.e. delete each task and clear all lists.
     **/
    void clearAll();

    /**
     * Delete each task in 'list' and clear the list.
     **/
    void nuke( PkgTaskList & list );

    /**
     * Return the mutex for this object to protect access from different
     * threads. Use a QMutexLocker to ensure it doesn't remain locked.
     **/
    QMutex & mutex() { return _mutex; }

protected:

    PkgTaskList _todo;
    PkgTaskList _doing;
    PkgTaskList _done;
    PkgTaskList _failed;

    QMutex      _mutex;
};

#endif // PkgTasks_h
