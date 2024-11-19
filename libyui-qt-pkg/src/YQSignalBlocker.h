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

  File:	      YQSignalBlocker.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQSignalBlocker_h
#define YQSignalBlocker_h

class QObject;

/**
 * Helper class to block Qt signals for QWidgets or QObjects as long as this
 * object exists.
 *
 * This object will restore the old blocked state when it goes out of scope.
 **/
class YQSignalBlocker
{
public:

    /**
     * Constructor.
     *
     * Qt signals for 'qobject' will be blocked as long as this object exists.
     * Remember that a QWidget is also a QObject.
     **/
    YQSignalBlocker( QObject * qobject );

    /**
     * Destructor.
     *
     * This will restore the old signal state.
     **/
    ~YQSignalBlocker();


private:

    QObject *	_qobject;
    bool	_oldBlockedState;
};


#endif // ifndef YQSignalBlocker_h
