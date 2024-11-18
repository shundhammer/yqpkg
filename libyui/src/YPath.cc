/*
  Copyright (c) 2012 Björn Esser

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
  SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


/*-/

  File:		YPath.cc

  Author:	Björn Esser <bjoern.esser@gmail.com>

/-*/

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

#include "YPath.h"
#include "YSettings.h"
#include "Libyui_config.h"

#define YUILogComponent "ui"
#include "YUILog.h"

using std::string;
using std::vector;


YPath::YPath ( const string & directory, const string & filename )
{
    // yuiDebug() << "Given filename: " << filename << endl;

    bool	   isThemeDir	     = ! directory.compare ( THEMEDIR );
    string	   progSubDir	     = YSettings::progDir ();
    string	   fullname	     = "";
    string	   themeSubDir	     = "/current";
    size_t	   splitPos	     = fullPath.rfind( "/" );
    bool	   hasProgSubDir     = progSubDir.compare ( "" );
    bool	   hasSubDirPrepend  = ( splitPos != string::npos );
    string	   filenameNoPrepend = filename.substr ( splitPos + 1, string::npos );
    string	   subDirPrepend     = "";
    vector<string> dirList;

    if ( hasSubDirPrepend )
	subDirPrepend = filename.substr ( 0, splitPos );

    // yuiDebug() << "Preferring subdir: "	    << progSubDir << endl;
    // yuiDebug() << "Subdir given with filename: " << subDirPrepend << endl;
    // yuiDebug() << "Looking for: "		    << filenameNoPrepend << endl;

    if ( hasSubDirPrepend )	// prefer subdir prepended to filename
    {
	if ( isThemeDir )      // prefer /current inside THEMEDIR
	{
	    if ( hasProgSubDir )
		dirList.push_back( directory + "/" + progSubDir + themeSubDir + "/" + subDirPrepend );

	    dirList.push_back( directory + themeSubDir + "/" + subDirPrepend );
	}
	if ( hasProgSubDir )
	    dirList.push_back( directory + "/" + progSubDir + "/" + subDirPrepend );

	dirList.push_back( directory + "/" + subDirPrepend );
    }

    if ( isThemeDir )		// prefer /current inside THEMEDIR
    {
	if ( hasProgSubDir )
	    dirList.push_back( directory + "/" + progSubDir + themeSubDir );

	dirList.push_back( directory + themeSubDir );
    }

    // the "usual" lookup
    if ( hasProgSubDir )
	dirList.push_back( directory + "/" + progSubDir );

    dirList.push_back ( directory );

    for ( vector<string>::const_iterator x = dirList.begin ();
	  x != dirList.end () && fullPath.compare ( "" ) == 0 ;
	  ++x )
    {
	vector<string> fileList = lsDir( *x );

	for ( vector<string>::const_iterator i = fileList.begin ();
	      i != fileList.end () && fullPath.compare ( "" ) == 0 ;
	      ++i )
	{
	    if ( *i != "." && *i != ".." )		// filter out parent and curdir
	    {
		fullname =  directory + "/" + *i;
		if ( *i == filenameNoPrepend )
		    fullPath = fullname;
		else
		{
		    fullPath = lookRecursive ( fullname, filenameNoPrepend );
		}
	    }
	}
    }

    if ( fullPath.compare( "" ) != 0 )
    {
	// yuiDebug() << "Found " << filenameNoPrepend << " in " << dir() << endl;
    }
    else
    {
	// yuiDebug() << "Could NOT find " << filename << " by looking recursive inside " << directory << endl;
	fullPath = filename;
    }
}


YPath::~YPath()
{
}


vector<string> YPath::lsDir( const string & directory )
{
    vector<string>  fileList;
    DIR *	    dir;
    struct dirent * ent;

    if ( ( dir = opendir( directory.c_str () ) ) != NULL )
    {
	// yuiDebug() << "Looking in " << directory << endl;

	while ( ( ent = readdir( dir ) ) != NULL )
	    fileList.push_back( ent -> d_name );

	closedir ( dir );
    }

    return fileList;
}


string YPath::lookRecursive( const string & directory, const string & filename )
{
    vector<string> fileList = lsDir( directory );
    string	   file = "";
    string	   fullname;

    for ( vector<string>::const_iterator i = fileList.begin();
	  i != fileList.end() && file.compare ( "" ) == 0;
	  ++i )
    {
	if ( *i != "." && *i != ".." )		  // filter out parent and curdir
	{
	    fullname =	directory + "/" + ( *i );

	    if ( *i == filename )
		file = fullname;
	    else
	    {
		file = lookRecursive ( fullname, filename );
	    }
	}
    }

    return file;
}


string YPath::path()
{
    return fullPath;
}


string YPath::dir()
{
    return fullPath.substr ( 0, fullPath.rfind( "/" ) );
}
