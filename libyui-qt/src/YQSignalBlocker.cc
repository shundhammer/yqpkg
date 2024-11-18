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

  File:	      YQSignalBlocker.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include "YQSignalBlocker.h"
#include <qobject.h>


YQSignalBlocker::YQSignalBlocker( QObject * qobject )
    : _qobject( qobject )
{
    if ( _qobject )
    {
	_oldBlockedState = _qobject->signalsBlocked();
	_qobject->blockSignals( true );
    }
}


YQSignalBlocker::~YQSignalBlocker()
{
    if ( _qobject )
	_qobject->blockSignals( _oldBlockedState );
}

