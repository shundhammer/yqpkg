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
#include "utf8.h"
#include "YQZypp.h"
#include "PkgTasks.h"


bool PkgTask::operator==( const PkgTask & other ) const
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
    _todo.clear();
    _doing.clear();
    _done.clear();
    _failed.clear();
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


void PkgTasks::initFromZypp()
{
    clearAll();

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel selectable = *it;
        CHECK_PTR( selectable );;

        PkgTaskAction    action = PkgNoAction;
        PkgTaskRequester req    = PkgReqNone;

        switch ( selectable->status() )
        {
            case S_Install:     req = PkgReqUser; action = PkgInstall; break;
            case S_Update:      req = PkgReqUser; action = PkgUpdate;  break;
            case S_Del:         req = PkgReqUser; action = PkgRemove;  break;

            case S_AutoInstall: req = PkgReqDep;  action = PkgInstall; break;
            case S_AutoUpdate:  req = PkgReqDep;  action = PkgUpdate;  break;
            case S_AutoDel:     req = PkgReqDep;  action = PkgRemove;  break;

            case S_NoInst:
            case S_KeepInstalled:
            case S_Protected:
            case S_Taboo:
                action = PkgNoAction;
                break;

                // Intentionally omitting 'default' branch so the compiler can
                // catch unhandled enum states
        }

        if ( action != PkgNoAction )
        {
            QString name = fromUTF8( selectable->name() );
            _todo.add( PkgTask( name, action, req ) );
        }
    }
}

