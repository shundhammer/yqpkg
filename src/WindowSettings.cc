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


#include <QSettings>
#include <QWidget>

#include "WindowSettings.h"
#include "Exception.h"
#include "Logger.h"


void WindowSettings::read( QWidget *       widget,
                           const QString & settingsGroup )
{
    QSettings settings;
    settings.beginGroup( settingsGroup );

    QPoint winPos	 = settings.value( "WindowPos" , QPoint( -99, -99 ) ).toPoint();
    QSize  winSize	 = settings.value( "WindowSize", QSize (   0,   0 ) ).toSize();

    if ( winSize.height() > 100 && winSize.width() > 100 )
        widget->resize( winSize );

    if ( winPos.x() != -99 && winPos.y() != -99 )
        widget->move( winPos );

    settings.endGroup();
}


void WindowSettings::write( QWidget *       widget,
                            const QString & settingsGroup )
{
    QSettings settings;
    settings.beginGroup( settingsGroup );

    settings.setValue( "WindowPos" , widget->pos()  );
    settings.setValue( "WindowSize", widget->size() );

    settings.endGroup();
}

