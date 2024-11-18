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

  File:		YQGraphPluginStub.cc

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#include "YQGraphPluginStub.h"

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#define PLUGIN_BASE_NAME "qt-graph"

using std::string;



YQGraphPluginStub::YQGraphPluginStub()
    : YGraphPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
                       << " plugin successfully from " << pluginLibFullPath()
                       << endl;
    }

    impl = (YQGraphPluginIf*) locateSymbol("GP");

    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide GP symbol" << endl;
    }
}


YQGraphPluginStub::~YQGraphPluginStub()
{
    // NOP
}


YGraph *
YQGraphPluginStub::createGraph( YWidget *      parent,
                                const string & filename,
				const string & layoutAlgorithm )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

    return impl->createGraph( parent, filename, layoutAlgorithm );
}


YGraph *
YQGraphPluginStub::createGraph( YWidget * parent, /* graph_t */ void * graph )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

    return impl->createGraph( parent, graph );
}
