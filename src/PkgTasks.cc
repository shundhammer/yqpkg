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
#include "PkgTasks.h"


bool PkgTask::operator==( const PkgTask & other )
{
    return _name      == other.name()
        && _action    == other.action()
        && _requester == other.requester();
}


void PkgTaskList::add( const PkgTask & task )
{
    if ( ! contains( task ) )
        append( task );
    else
        logError() << "Task " << task.name() << " is already in the list" << endl;
}


PkgTaskList
PkgTaskList::filtered( PkgTaskAction    filterAction,
                       PkgTaskRequester filterRequester ) const
{
    PkgTaskList result;

    foreach( const PkgTask & task, *this )
    {
        if ( ( task.action()    & filterAction    ) > 0 &&
             ( task.requester() & filterRequester ) > 0    )
        {
            result << task;
        }
    }

    return result;
}




void PkgTasks::clearAll()
{
    _toDo.clear();
    _doing.clear();
    _done.clear();
    _failed.clear();
}


void PkgTasks::initFromZypp()
{
    clearAll();

    // Iterate over all zypp selectables
}


void PkgTasks::moveTask( const PkgTask & task,
                         PkgTaskList &   fromList,
                         PkgTaskList &   toList )
{
    int index = fromList.indexOf( task );

    if ( index < 0 )
    {
        logError() << "Task " << task.name() << " not found in this list" << endl;
        return;
    }

    toList.add( fromList.takeAt( index ) );
}

