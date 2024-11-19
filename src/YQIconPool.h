/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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
  File:	      YQIconPool.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQIconPool_h
#define YQIconPool_h

#include <qpixmap.h>
#include <QHash>

class YQIconPool
{
public:

    static QPixmap pkgAutoDel();
    static QPixmap pkgAutoInstall();
    static QPixmap pkgAutoUpdate();
    static QPixmap pkgDel();
    static QPixmap pkgInstall();
    static QPixmap pkgKeepInstalled();
    static QPixmap pkgNoInst();
    static QPixmap pkgProtected();
    static QPixmap pkgTaboo();
    static QPixmap pkgUpdate();

    static QPixmap disabledPkgAutoDel();
    static QPixmap disabledPkgAutoInstall();
    static QPixmap disabledPkgAutoUpdate();
    static QPixmap disabledPkgDel();
    static QPixmap disabledPkgInstall();
    static QPixmap disabledPkgKeepInstalled();
    static QPixmap disabledPkgNoInst();
    static QPixmap disabledPkgProtected();
    static QPixmap disabledPkgTaboo();
    static QPixmap disabledPkgUpdate();

    static QPixmap normalPkgConflict();

    static QPixmap treePlus();
    static QPixmap treeMinus();

    static QPixmap warningSign();
    static QPixmap pkgSatisfied();

    static QPixmap tabRemove();
    static QPixmap arrowLeft();
    static QPixmap arrowRight();
    static QPixmap arrowDown();

protected:

    /**
     * Return the global icon pool.
     **/
    static YQIconPool * iconPool();

    /**
     * Return the cached icon for 'icon_name'. If the icon isn't in the cache
     * yet, load it and store it in the cache.
     *
     * Return a red square as an error icon if there is no icon by that name.
     **/
    QPixmap cachedIcon(const QString icon_name, const bool enabled );

    /**
     * Load the icon for 'icon_name' from the icon theme or, if that fails,
     * from the compiled-in icons (using the Qt resource system). Return a null
     * pixmap if there is no such icon.
     **/
    QPixmap loadIcon( const QString icon_name, const bool enabled );

private:

    /**
     * (Private!) Constructor
     * Singleton object - use the static methods only!
     **/
    YQIconPool();

    /**
     * Destructor
     **/
    virtual ~YQIconPool();


    //
    // Data members
    //

    static YQIconPool * _iconPool;
    QHash< const QString, QPixmap >	_iconCache;
};


#endif // ifndef YQIconPool_h
