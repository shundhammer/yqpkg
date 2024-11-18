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

  File:		YQPackageSelectorPluginStub.cc

  Authors:	Katharina Machalkova <kmachalkova@suse.cz>
		Stephan Kulow <coolo@suse.de>

/-*/


#include "YQPackageSelectorPluginStub.h"

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#define PLUGIN_BASE_NAME "qt-pkg"


YQPackageSelectorPluginStub::YQPackageSelectorPluginStub()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
                       << " plugin successfully from " << pluginLibFullPath()
                       << endl;
    }


    impl = (YQPackageSelectorPluginIf*) locateSymbol("PSP");

    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide PSP symbol" << endl;
    }
}


YQPackageSelectorPluginStub::~YQPackageSelectorPluginStub()
{
    // NOP
}


YPackageSelector *
YQPackageSelectorPluginStub::createPackageSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

    return impl->createPackageSelector( parent, modeFlags );
}


YWidget *
YQPackageSelectorPluginStub::createPatternSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

    return impl->createPatternSelector( parent, modeFlags );
}


YWidget *
YQPackageSelectorPluginStub::createSimplePatchSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

    return impl->createSimplePatchSelector( parent, modeFlags );
}

