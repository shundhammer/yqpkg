/*
  Copyright (c) 2024 SUSE LLC
  
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

  File:		QY2CursorHelper.cc

  Author:	Stefan Hundhammer <shundhammer@suse.com>

  Textdomain	"qt"

/-*/

#include <QCursor>
#include "QY2CursorHelper.h"


void busyCursor()
{
    // qApp is Qt's global QApplication instance
    qApp->setOverrideCursor( Qt::BusyCursor );
}


void normalCursor()
{
    // qApp is Qt's global QApplication instance
    
    while ( qApp->overrideCursor() )
	qApp->restoreOverrideCursor();
}
