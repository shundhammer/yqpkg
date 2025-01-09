/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


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
