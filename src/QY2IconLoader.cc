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


#include <unistd.h>     // access() / R_OK

#include "Logger.h"
#include "QY2IconLoader.h"


QIcon QY2IconLoader::loadIcon( const QString & iconName )
{
    // Notice that QIcon::isNull() is not a useful check if loading an icon was
    // successful: It returns 'false' even if the icon just has a filename.
    //
    // There does not seem any reliable check if loading an icon was successful.

    if ( QIcon::hasThemeIcon( iconName ) )
        return loadThemeIcon( iconName );

    if ( iconName.startsWith( "/" ) )
        return loadIconFromPath( iconName );

    return loadBuiltInIcon( iconName );
}


QIcon QY2IconLoader::loadThemeIcon( const QString & iconName )
{
    logVerbose() << "Using theme icon for " << iconName << endl;
    return QIcon::fromTheme( iconName );
}


QIcon QY2IconLoader::loadIconFromPath( const QString & iconName )
{
    if ( access( iconName.toUtf8(), R_OK ) != 0 )
    {
        logWarning() << "Can't open icon file " << iconName << endl;
        return QIcon();
    }

    logVerbose() << "Loading icon from absolute path " << iconName << endl;
    return QIcon( iconName );
}


QIcon QY2IconLoader::loadBuiltInIcon( const QString & iconName )
{
    QString resource = ":/";

    QString iconPath = iconName.startsWith( resource ) ?
        iconName : resource + iconName;

    if ( QFile( iconPath ).exists() )
        // Using QFile because it can handle Qt resources and their aliases
    {
        logVerbose() << "Using built-in icon " << iconName << endl;
        return QIcon( iconPath );
    }
    else
    {
        logWarning() << "No built-in icon " << iconName << endl;
        return QIcon();
    }
}
