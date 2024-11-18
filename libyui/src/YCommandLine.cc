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

  File:		YCommandLine.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <stdlib.h>	// malloc()
#include <string.h>	// strdup()

#include <vector>
#include <fstream>

#include "YCommandLine.h"
#include "YUIException.h"

#define YUILogComponent "ui"
#include "YUILog.h"

using std::string;


struct YCommandLinePrivate
{
    std::vector<string> args;
};





YCommandLine::YCommandLine()
    : priv( new YCommandLinePrivate() )
{
    YUI_CHECK_NEW( priv );

    std::ifstream cmdline( "/proc/self/cmdline", std::ifstream::in | std::ifstream::binary );

    while ( cmdline.good() )
    {
	string arg;
	getline( cmdline, arg, '\0' );

	if ( ! arg.empty() )
	{
	    yuiDebug() << "Arg #" << priv->args.size()
		       << ": \"" << arg << "\"" << endl;

	    priv->args.push_back( arg );
	}
    }
}


YCommandLine::~YCommandLine()
{

}


int
YCommandLine::argc() const
{
    return priv->args.size();
}


char **
YCommandLine::argv() const
{
    char ** argArray = (char **) ( malloc( argc() * sizeof( char * ) ) );

    if ( argArray )
    {
	for ( int i=0; i < argc(); i++ )
	{
	    argArray[ i ] = strdup( priv->args[i].c_str() );
	}
    }

    return argArray;
}


void
YCommandLine::add( const string & arg )
{
    priv->args.push_back( arg );
}


string
YCommandLine::arg( int index ) const
{
    YUI_CHECK_INDEX( index, 0, (int) priv->args.size()-1 );

    return priv->args[ index ];
}


void
YCommandLine::remove( int index )
{
    YUI_CHECK_INDEX( index, 0, (int) priv->args.size()-1 );

    priv->args.erase( priv->args.begin() + index );
}


void
YCommandLine::replace( int index, const string & newArg )
{
    YUI_CHECK_INDEX( index, 0, (int) priv->args.size()-1 );

    priv->args[ index ] = newArg;
}


int
YCommandLine::find( const string & argName ) const
{
    for ( int i=0; i < argc(); i++ )
    {
	if ( priv->args[i] == argName )
	    return i;
    }

    return -1;
}
