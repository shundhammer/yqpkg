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

  File:		YUIPlugin.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <dlfcn.h>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUIPlugin.h"
#include "YPath.h"

#include "Libyui_config.h"

using std::string;



YUIPlugin::YUIPlugin( const char * pluginLibBaseName )
{
    _pluginLibBaseName = string( pluginLibBaseName );

    string pluginFilename = pluginLibFullPath();

    _pluginLibHandle = dlopen( pluginFilename.c_str(),
			       RTLD_NOW | RTLD_GLOBAL);

    if ( ! _pluginLibHandle )
    {
	_errorMsg = dlerror();

	yuiError() << "Could not load UI plugin \"" << pluginLibBaseName
		   << "\": " << _errorMsg
		   << endl;
    }
}


YUIPlugin::~YUIPlugin()
{
    // This intentionally does NOT call unload(): This would be
    // counterproductive for almost all use cases of this class.
}


void
YUIPlugin::unload()
{
    if ( _pluginLibHandle )
	dlclose( _pluginLibHandle );
}


string
YUIPlugin::pluginLibFullPath() const
{

    string pluginName = PLUGIN_PREFIX;
    pluginName.append( _pluginLibBaseName );
    pluginName.append( PLUGIN_SUFFIX );

    YPath plugin( PLUGINDIR, pluginName );

    return plugin.path();
}


void * YUIPlugin::locateSymbol( const char * symbol )
{
    if ( ! _pluginLibHandle )
	return 0;

    void * addr = dlsym( _pluginLibHandle, symbol );

    if ( ! addr )
    {
	yuiError() << "Could not locate symbol \"" << symbol
		   << "\" in " << pluginLibFullPath()
		   << endl;
    }

    return addr;
}


bool YUIPlugin::error() const
{
    return _pluginLibHandle == 0;
}


bool YUIPlugin::success() const
{
    return _pluginLibHandle != 0;
}


string YUIPlugin::errorMsg() const
{
    return _errorMsg;
}
