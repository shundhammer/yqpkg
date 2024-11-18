/*
  Copyright (C) 2000-2012 Novell, Inc
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

  File:		YIconLoader.cc

  Author:	Katarína Machálková <kmachalkova@novell.com>

/-*/


#include <sys/stat.h>
#include <sstream>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YIconLoader.h"

#define FALLBACK_ICON_PATH "/usr/share/icons/hicolor/"


using std::string;


YIconLoader::YIconLoader()
{
    addIconSearchPath( FALLBACK_ICON_PATH );
}


YIconLoader::~YIconLoader()
{
}


void YIconLoader::setIconBasePath( string path )
{
    _iconBasePath = path;
}


string YIconLoader::iconBasePath() const
{
    return _iconBasePath;
}


void YIconLoader::addIconSearchPath( string path )
{
    _iconDirs.push_front( path );
}


string YIconLoader::findIcon( string name )
{
    // No extension -> add some
    string::size_type loc = name.find( ".png" );
    if ( loc == string::npos )
	name += ".png";

    // Absolute path -> return it
    if ( name[0] == '/' )
	return name;

    string fullPath;

    // Look in global search path
    if ( !_iconBasePath.empty () )
    {
	fullPath = _iconBasePath + name;
	if ( fileExists ( fullPath ) )
	{
	    // yuiMilestone() << "Found " << name << " in global search path" << endl;
	    return fullPath;
	}
    }

    // Now search the fallback dirs
    std::list<string>::iterator listIt = _iconDirs.begin();

    while ( listIt != _iconDirs.end() )
    {
	// Something like relative path
	if ( name.find('/') != string::npos )
	    fullPath = *listIt +  name;
	// No '/' chars, just the name -> use '22x22/apps' fallback
	else
	    fullPath = *listIt + "22x22/apps/" + name;

	if ( fileExists( fullPath ) )
	{
	    // yuiMilestone() << "Found " << name << " in " <<  *listIt << " search path" << endl;
	    return fullPath;
	}

	yuiDebug() <<  name << " not found in " << *listIt << " search path, skipping" << endl;
	listIt++;
    }

    return "";
}


bool YIconLoader::fileExists( string fname )
{
    struct stat fileInfo;
    int ret = stat( fname.c_str(), &fileInfo );

    return ret == 0;
}
