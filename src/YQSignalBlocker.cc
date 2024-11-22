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

