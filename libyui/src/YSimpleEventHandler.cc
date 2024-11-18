/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		YEvent.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui-events"
#include "YUILog.h"

#include "YEvent.h"
#include "YSimpleEventHandler.h"


#define VERBOSE_EVENTS	0
#define VERBOSE_BLOCK	0


YSimpleEventHandler::YSimpleEventHandler()
{
    _pendingEvent	= 0;
    _eventsBlocked	= false;
}


YSimpleEventHandler::~YSimpleEventHandler()
{
    clear();
}


void YSimpleEventHandler::clear()
{
    if ( _pendingEvent )
    {
#if VERBOSE_EVENTS
	yuiDebug() << "Clearing pending event: " << _pendingEvent << endl;
#endif
	deleteEvent( _pendingEvent );
    }
}


YEvent * YSimpleEventHandler::consumePendingEvent()
{
    YEvent * event = _pendingEvent;
    _pendingEvent = 0;

#if VERBOSE_EVENTS
    yuiDebug() << "Consuming " << event << endl;
#endif

    return event;
}


void YSimpleEventHandler::sendEvent( YEvent * event )
{
    if ( ! event )
    {
	yuiError() << "Ignoring NULL event" << endl;
	return;
    }

    if ( eventsBlocked() )
    {
#if VERBOSE_BLOCK
	yuiDebug() << "Blocking " << event << endl;
#endif
	// Avoid memory leak: The event handler assumes ownership of the newly
	// created event, so we have to clean it up here.
	deleteEvent( event );

	return;
    }

    if ( _pendingEvent )
    {
	/**
	 * This simple event handler keeps track of only the latest user event.
	 * If there is more than one, older events are automatically discarded.
	 * Since Events are created on the heap with the "new" operator,
	 * discarded events need to be deleted.
	 *
	 * Events that are not discarded are deleted later (after they are
	 * processed) by the generic UI.
	 **/

	deleteEvent( _pendingEvent );
    }

#if VERBOSE_EVENTS
    yuiDebug() << "New pending event: " << event << endl;
#endif

    _pendingEvent = event;
}


bool
YSimpleEventHandler::eventPendingFor( YWidget * widget ) const
{
    YWidgetEvent * event = dynamic_cast<YWidgetEvent *> (_pendingEvent);

    if ( ! event )
	return false;

    return event->widget() == widget;
}


void YSimpleEventHandler::deletePendingEventsFor( YWidget * widget )
{
    if ( ! _pendingEvent )
	return;

    YWidgetEvent * event = dynamic_cast<YWidgetEvent *> (_pendingEvent);

    if ( event && event->widget() == widget && event->isValid() )
    {
	yuiDebug() << "Deleting " << _pendingEvent << endl;
	deleteEvent( _pendingEvent );
    }
}


void YSimpleEventHandler::blockEvents( bool block )
{
#if VERBOSE_BLOCK
    if ( block )	yuiDebug() << "Blocking events"   << endl;
    else		yuiDebug() << "Unblocking events" << endl;
#endif

    _eventsBlocked = block;
}


void YSimpleEventHandler::deleteEvent( YEvent * event )
{
    if ( event == _pendingEvent )
	_pendingEvent = 0;

    if ( event )
    {
	if ( event->isValid() )
	{
#if VERBOSE_EVENTS
	    yuiDebug() << "Deleting " << event << endl;
#endif
	    delete event;
	}
	else
	{
	    yuiError() << "Attempt to delete invalid event " << event << endl;
	}
    }
}
