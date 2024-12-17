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
        , _nextSerial( 0 )
        , _sortByInsertionSequence( true )
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

    /**
     * Return 'true' if the sort order should always be the item insertion
     * order, 'false' if default sort order.
     **/
    bool sortByInsertionSequence() const { return _sortByInsertionSequence; }

    /**
     * Enforce sorting by item insertion order (true) default sort order.
     **/
    virtual void setSortByInsertionSequence( bool value )
        { _sortByInsertionSequence = value; }

    /**
     * Return the next free serial number for items that want to be ordered in
     * insertion sequence.
     **/
    int nextSerial() { return _nextSerial++; }


protected:

    int  _nextSerial;
    bool _sortByInsertionSequence;
};


class PkgTaskListWidgetItem: public QListWidgetItem
{
public:

    PkgTaskListWidgetItem( PkgTask *           task,
                           PkgTaskListWidget * parent = 0 );

    PkgTask * task() const { return _task; }

    /**
     * Comparison function used for sorting the list.
     * Reimplemented from QListWidgetItem.
     **/
    virtual bool operator<( const QListWidgetItem & other ) const override;

    /**
     * Return this item's serial number in its original parent list widget.
     * Notice that this is set only only once during creation by default.
     **/
    int serial() const { return _serial; }

    /**
     * Set a new serial number. This can be necessary if the item is moved from
     * one list widget to another, or if the item was created without a parent.
     **/
    void setSerial( int value ) { _serial = value; }

protected:

    /**
     * Check the parent PkgTaskListWidget if items should be sorted by
     * insertion sequence.
     **/
    bool sortByInsertionSequence() const;


    PkgTask * _task;
    int       _serial;
};


#endif // PkgTaskListWidget_h
