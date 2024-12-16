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


#ifndef PkgTaskListWidget_h
#define PkgTaskListWidget_h


#include <QListWidget>
#include "PkgTasks.h"

class PkgTaskListWidgetItem;


/**
 * A QListWidget specialized for PkgTasks.
 **/
class PkgTaskListWidget: public QListWidget
{
    Q_OBJECT

public:

    PkgTaskListWidget( QWidget * parent )
        : QListWidget( parent )
        {}

    virtual ~PkgTaskListWidget() {}

    /**
     * Clear all existing items and Add all tasks from 'taskList'.
     **/
    void addTaskItems( const PkgTaskList & taskList );

    /**
     * Add an item for a single task and return it.
     **/
    PkgTaskListWidgetItem * addTaskItem( PkgTask * task );

    /**
     * Remove an item for a task.
     **/
    void removeTaskItem( PkgTask * task );

    /**
     * Find the list widget item for a task and return it.
     * Return 0 if not found.
     *
     * Ownership of the item remains with the list widget.
     **/
    PkgTaskListWidgetItem * findTaskItem( PkgTask * task ) const;
};


class PkgTaskListWidgetItem: public QListWidgetItem
{
public:

    PkgTaskListWidgetItem( PkgTask *     task,
                           QListWidget * parent = 0 );

    PkgTask * task() const { return _task; }

protected:

    PkgTask * _task;
};


#endif // PkgTaskListWidget_h
