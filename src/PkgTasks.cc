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



bool PkgTask::matches( const QString &  name,
                       PkgTaskAction    action,
                       PkgTaskRequester requester ) const
{
    // Check action and requester.
    //
    // Using bitwise & to be able to use OR'ed values like
    // PkgAdd,    i.e.  PkgInstall | PkgUpdate, or
    // PkgReqAll, i.e.  PkgReqUser | PkgReqDep.

    bool match =
        ( this->_action    & action    ) &&
        ( this->_requester & requester );

    // Check the name unless empty.

    if ( match && ! name.isEmpty() )
        match = ( this->_name == name );

    return match;
}


bool PkgTask::matches( PkgTask * other ) const
{
    if ( ! other )
        return false;

    return matches( other->name(), other->action(), other->requester() );
}


bool PkgTask::matches( const PkgTask & other ) const
{
    return matches( other.name(), other.action(), other.requester() );
}


//----------------------------------------------------------------------


PkgTaskList::PkgTaskList( const QString & listName )
        : QList<PkgTask *>()
        , _name( listName )
{
}


PkgTaskList::~PkgTaskList()
{
}


PkgTask *
PkgTaskList::find( const QString &  name,
                   PkgTaskAction    action,
                   PkgTaskRequester requester ) const
{
    foreach ( PkgTask * task, *this )
    {
        if ( task && task->matches( name, action, requester ) )
            return task;
    }

    return 0;
}


PkgTask *
PkgTaskList::find( const PkgTask & filter ) const
{
    foreach ( PkgTask * task, *this )
    {
        if ( task && task->matches( filter ) )
            return task;
    }

    return 0;
}


PkgTask *
PkgTaskList::find( ZyppRes zyppRes ) const
{
    QString resName = fromUTF8( zyppRes->name() );

    foreach ( PkgTask * task, *this )
    {
        if ( task && task->name() == resName )
            return task;
    }

    return 0;
}


PkgTaskList
PkgTaskList::filtered( PkgTaskAction    filterAction,
                       PkgTaskRequester filterRequester )
{
    PkgTaskList result( QString( "filtered %1" ).arg( _name ) );

    foreach( const PkgTask * task, *this )
    {
        if ( ( task->action()    & filterAction    ) &&
             ( task->requester() & filterRequester )    )
        {
            result.append( new PkgTask( *task ) );
        }
    }

    return result;
}


//----------------------------------------------------------------------


PkgTasks::PkgTasks()
    : _todo  ( "todo" )
    , _doing ( "doing" )
    , _done  ( "done" )
    , _failed( "failed" )
{
    logDebug() << endl;
}


PkgTasks::~PkgTasks()
{
    clearAll();

    logDebug() << "Destroying PkgTasks done" << endl;
}


void PkgTasks::clearAll()
{
    nuke( _todo   );
    nuke( _doing  );
    nuke( _done   );
    nuke( _failed );
}


void PkgTasks::nuke( PkgTaskList & list )
{
    if ( ! list.isEmpty() )
    {
        logDebug() << "Nuking PkgTaskList \"" << list.name()
                   << "\"  size:" << list.size()
                   << endl;

        qDeleteAll( list );
        list.clear();
    }
}


void PkgTasks::moveTask( PkgTask *      task,
                         PkgTaskList &  fromList,
                         PkgTaskList &  toList )
{
    int index = fromList.indexOf( task );

    if ( index < 0 )
    {
        logError() << "Task " << task->name() << " not found in this list" << endl;
        return;
    }

    toList.append( fromList.takeAt( index ) );
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
            QString   name = fromUTF8( selectable->name() );
            PkgTask * task = new PkgTask( name, action, req );
            CHECK_NEW( task );

            task->setCompletedPercent( 0 );

            if ( action & PkgAdd ) // PkgInstall | PkgUpdate
            {
                const ZyppObj candidate = selectable->candidateObj();

                if ( candidate )
                {
                    task->setInstalledSize( candidate->installSize() );
                    task->setDownloadSize ( candidate->downloadSize() );
                    task->setDownloadedPercent( 0 );
                }
            }
            else if ( action & PkgRemove )
            {
                task->setDownloadSize( 0.0 );

                const ZyppObj installed = selectable->installedObj();

                if ( installed )
                    task->setInstalledSize( installed->installSize() );
            }

            _todo.append( task );
        }
    }
}

