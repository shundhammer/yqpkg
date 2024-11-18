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
  File:	      YQIconPool.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <yui/qt/utf8.h>

#include <QPixmap>
#include <QIcon>

#include "YQIconPool.h"


using std::endl;



YQIconPool * YQIconPool::_iconPool = 0;

QPixmap YQIconPool::pkgTaboo()			{ return iconPool()->cachedIcon( "package-available-locked", true );		}
QPixmap YQIconPool::pkgDel()			{ return iconPool()->cachedIcon( "package-remove", true );			}
QPixmap YQIconPool::pkgUpdate()			{ return iconPool()->cachedIcon( "package-upgrade", true );			}
QPixmap YQIconPool::pkgInstall()		{ return iconPool()->cachedIcon( "package-install", true );			}
QPixmap YQIconPool::pkgAutoInstall()		{ return iconPool()->cachedIcon( "package-install-auto", true );		}
QPixmap YQIconPool::pkgAutoUpdate()		{ return iconPool()->cachedIcon( "package-upgrade-auto", true );		}
QPixmap YQIconPool::pkgAutoDel()		{ return iconPool()->cachedIcon( "package-remove-auto", true );			}
QPixmap YQIconPool::pkgKeepInstalled()		{ return iconPool()->cachedIcon( "package-installed-updated", true );		}
QPixmap YQIconPool::pkgNoInst()			{ return iconPool()->cachedIcon( "package-available", true );			}
QPixmap YQIconPool::pkgProtected()		{ return iconPool()->cachedIcon( "package-installed-locked", true );		}

QPixmap YQIconPool::disabledPkgTaboo()		{ return iconPool()->cachedIcon( "package-available-locked", false );		}
QPixmap YQIconPool::disabledPkgDel()		{ return iconPool()->cachedIcon( "package-remove", false );			}
QPixmap YQIconPool::disabledPkgUpdate()		{ return iconPool()->cachedIcon( "package-upgrade", false );			}
QPixmap YQIconPool::disabledPkgInstall()	{ return iconPool()->cachedIcon( "package-install", false );			}
QPixmap YQIconPool::disabledPkgAutoInstall()	{ return iconPool()->cachedIcon( "package-install-auto", false );		}
QPixmap YQIconPool::disabledPkgAutoUpdate()	{ return iconPool()->cachedIcon( "package-upgrade-auto", false );		}
QPixmap YQIconPool::disabledPkgAutoDel()	{ return iconPool()->cachedIcon( "package-remove-auto", false );		}
QPixmap YQIconPool::disabledPkgKeepInstalled()	{ return iconPool()->cachedIcon( "package-installed-updated", false );		}
QPixmap YQIconPool::disabledPkgNoInst()		{ return iconPool()->cachedIcon( "package-available", false );			}
QPixmap YQIconPool::disabledPkgProtected()	{ return iconPool()->cachedIcon( "package-installed-locked", false );		}

QPixmap YQIconPool::normalPkgConflict()		{ return iconPool()->cachedIcon( "emblem-warning", true );			}

QPixmap YQIconPool::treePlus()			{ return iconPool()->cachedIcon( "list-add", true );				}
QPixmap YQIconPool::treeMinus()			{ return iconPool()->cachedIcon( "list-remove", true );				}

QPixmap YQIconPool::warningSign()		{ return iconPool()->cachedIcon( "emblem-warning", true );			}
QPixmap YQIconPool::pkgSatisfied()		{ return iconPool()->cachedIcon( "package-supported", true );			}

QPixmap YQIconPool::tabRemove()			{ return iconPool()->cachedIcon( "tab-close", true );				}
QPixmap YQIconPool::arrowLeft()			{ return iconPool()->cachedIcon( "arrow-left", true );				}
QPixmap YQIconPool::arrowRight()		{ return iconPool()->cachedIcon( "arrow-right", true );				}
QPixmap YQIconPool::arrowDown()			{ return iconPool()->cachedIcon( "arrow-down", true );				}

YQIconPool * YQIconPool::iconPool()
{
    if ( ! _iconPool )
	_iconPool = new YQIconPool();

    return _iconPool;
}


YQIconPool::YQIconPool()
{
}


YQIconPool::~YQIconPool()
{
    // NOP
}



QPixmap
YQIconPool::cachedIcon( const QString icon_name, const bool enabled )
{
    Q_INIT_RESOURCE(qt_pkg_icons);
    QPixmap iconPixmap = _iconCache[ icon_name + enabled ];

    if ( !iconPixmap )
    {
        iconPixmap = loadIcon( icon_name, enabled );

        if ( !iconPixmap )
        {
            // Create an icon for the cache to avoid more than one complaint
            // and to have a clearly visible error icon (a small red square)
            iconPixmap = QPixmap( 8, 8 );
            iconPixmap.fill( Qt::red );
        }
    }

    _iconCache.insert( icon_name + enabled, iconPixmap );

    return iconPixmap;
}


QPixmap
YQIconPool::loadIcon( const QString icon_name, const bool enabled )
{
    QPixmap iconPixmap = _iconCache[ icon_name + enabled ];

    if ( QIcon::hasThemeIcon( icon_name ) )
    {
        yuiDebug() << "Loading theme icon " << icon_name << endl;

        QIcon icon = QIcon::fromTheme( icon_name, QIcon( ":/" + icon_name ) );
        iconPixmap = icon.pixmap( QSize( 16, 16 ), enabled ? QIcon::Normal : QIcon::Disabled );
    }
    else
    {
        yuiDebug() << "Loading built-in icon " << icon_name << endl;

        QIcon icon = QIcon( ":/" + icon_name );
        iconPixmap = icon.pixmap( QSize( 16, 16 ), enabled ? QIcon::Normal : QIcon::Disabled );
    }

    if ( !iconPixmap )
        yuiError() << "Could not load icon " << icon_name << endl;

    return iconPixmap;
}

