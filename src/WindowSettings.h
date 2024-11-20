/*
 *   File name: WindowSettings.h
 *   Summary:	Helper class for QSettings
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *              Donated by the QDirStat project.
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
