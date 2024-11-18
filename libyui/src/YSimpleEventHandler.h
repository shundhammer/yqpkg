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

  File:		YSimpleEventHandler.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSimpleEventHandler_h
#define YSimpleEventHandler_h


class YEvent;
class YWidget;


/**
 * Simple event handler suitable for most UIs.
 *
 * This event handler keeps track of one single event that gets overwritten
 * when a new one arrives.
 **/
class YSimpleEventHandler
{
public:

    /**
     * Constructor.
     **/
    YSimpleEventHandler();

    /**
     * Destructor.
     *
     * If there is a pending event, it is deleted here.
     **/
    virtual ~YSimpleEventHandler();

    /**
     * Widget event handlers call this when an event occured that
     * should be the answer to a UserInput() / PollInput() (etc.) call.
     *
     * The UI assumes ownership of the event object that 'event' points to, so
     * the event MUST be created with new(). The UI is to take care to delete
     * the event after it has been processed.
     *
     * If events are blocked (see blockEvents() ), the event sent with this
     * function will be ignored (but safely deleted - no memory leak).
     *
     * It is an error to pass 0 for 'event'.
     **/
    void sendEvent( YEvent * event_disown );

    /**
     * Returns 'true' if there is any event pending for the specified widget.
     **/
    bool eventPendingFor( YWidget * widget ) const;

    /**
     * Returns the last event that isn't processed yet or 0 if there is none.
     *
     * This event handler keeps track of only one single (the last one) event.
     **/
    YEvent * pendingEvent() const { return _pendingEvent; }

    /**
     * Consumes the pending event. Sets the internal pending event to 0.
     * Does NOT delete the internal consuming event.
     *
     * The caller assumes ownership of the object this pending event points
     * to. In particular, he has to take care to delete that object when he is
     * done processing it.
     *
     * Returns the pending event or 0 if there is none.
     **/
    YEvent * consumePendingEvent();

    /**
     * Delete any pending events for the specified widget. This is useful
     * mostly if the widget is about to be destroyed.
     **/
    void deletePendingEventsFor( YWidget * widget );

    /**
     * Clears any pending event (deletes the corresponding object).
     **/
    void clear();

    /**
     * Block (or unblock) events. If events are blocked, any event sent with
     * sendEvent() from now on is ignored (and will get lost) until events are
     * unblocked again.
     **/
    void blockEvents( bool block = true );

    /**
     * Unblock events previously blocked. This is just an alias for
     * blockEvents( false) for better readability.
     **/
    void unblockEvents() { blockEvents( false ); }

    /**
     * Returns 'true' if events are currently blocked.
     **/
    bool eventsBlocked() const { return _eventsBlocked; }

    /**
     * Delete an event. Don't call this from the outside; this is public only
     * because of limitations of C++ .
     **/
    void deleteEvent( YEvent * event );


protected:

    // Data members

    YEvent * 	_pendingEvent;
    bool	_eventsBlocked;
};




#endif // YSimpleEventHandler_h
