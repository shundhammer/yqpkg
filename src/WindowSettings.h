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


#ifndef WindowSettings_h
#define WindowSettings_h

#include <QString>

class QSettings;
class QWidget;


class WindowSettings
{
public:
    /**
     * Read window settings (size and position) from the settings and apply
     * them.
     **/
    static void read( QWidget *       widget,
                      const QString & settingsGroup );

    /**
     * Write window settings (size and position) to the settings.
     **/
    static void write( QWidget *       widget,
                       const QString & settingsGroup );

};	// namespace QDirStat

#endif	// WindowSettings_h
