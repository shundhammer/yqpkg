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


#include "Logger.h"
#include "Exception.h"
#include "PkgTaskListWidget.h"


void PkgTaskListWidget::addTaskItems( const PkgTaskList & taskList )
{
    foreach ( const PkgTask & task, taskList )
        addTaskItem( task );
}


void PkgTaskListWidget::addTaskItem( const PkgTask & task )
{
    PkgTaskListWidgetItem * item = new PkgTaskListWidgetItem( task, this );
    CHECK_NEW( item );
}


void PkgTaskListWidget::removeTaskItem( const PkgTask & task )
{
    for ( int row=0; row < count(); ++row )
    {
        PkgTaskListWidgetItem * it =
            dynamic_cast<PkgTaskListWidgetItem *>( item( row ) );

        if ( it && it->task() == task )
        {
            // This is actually the officially documented way to remove an item
            // from a QListWidget: Just delete it. It removes itself from its
            // parent QListView in its destructor. There is no call like
            // 'deleteItem()' or 'removeItem()'.

            delete it;
        }
    }
}




PkgTaskListWidgetItem::PkgTaskListWidgetItem( const PkgTask & task,
                                              QListWidget *   parent )
    : QListWidgetItem( parent )
    , _task( task )
{
    QString txt;

    if ( _task.action() == PkgRemove )
        txt = "- ";

    txt += _task.name();
    setText( txt );

    if ( parent )
        logDebug() << "New PkgTask item for " << parent->objectName()
                   << ": " << txt << endl;
}
