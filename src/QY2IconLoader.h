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


/*
  File:	      QY2IconLoader.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  These are pure Qt functions - they can be used independently of YaST2.
*/

#ifndef QY2IconLoader_h
#define QY2IconLoader_h

#include <QIcon>

/**
 * Helper class to load icons from various locations
 **/
class QY2IconLoader
{
public:
    /**
     * Load an icon. This tries several locations:
     *
     * - The icon theme from the current desktop
     * - The compiled-in Qt resources
     * - An external file
     *
     * If the icon does not have a filename extension, the icon theme will try
     * to append ".svg" and ".png". For the compiled-in Qt resources, there are
     * aliases specified ("foo" -> "foo.svg"), so it will also work without an
     * extension.
     *
     * For external files, a path and an extension will be necessary.
     *
     * If no icon could be loaded, this will return a null QIcon (check with
     * icon.isNull()), and a warning is logged.
     **/
    static QIcon loadIcon( const QString & iconName_str );

    /**
     * Load an icon from the desktop theme.
     * Those icon names typically do not contain a leading path or a filename extension.
     **/
    static QIcon loadThemeIcon( const QString & iconName );

    /**
     * Load an icon from an absolute path (including filename extension).
     **/
    static QIcon loadIconFromPath( const QString & iconName );

    /**
     * Load a built-in icon, i.e. one from the Qt resource system.
     *
     * Use either an alias from qt_icons.rcc (and no filename extension)
     * or a full filename with path and extension, e.g. "icons/foo.svg".
     * A leading ":/" will be added if not already there.
     **/
    static QIcon loadBuiltInIcon( const QString & iconName );
};

#endif // QY2IconLoader_h
