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

  File:		YQGraphPluginStub.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#ifndef YQGraphPluginStub_h
#define YQGraphPluginStub_h

#include <yui/YGraphPlugin.h>
#include <yui/YDialog.h>
#include <yui/YEvent.h>

#include "YQGraphPluginIf.h"


/**
 * Simplified access to the package selector plugin.
 **/
class YQGraphPluginIf;


class YQGraphPluginStub : public YGraphPlugin
{
public:
    /**
     * Constructor: Load the plugin library for the package selector.
     **/
    YQGraphPluginStub();

    /**
     * Destructor.
     **/
    virtual ~YQGraphPluginStub();

    /**
     * Create a package selector.
     * Implemented from YGraphPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YGraph * createGraph( YWidget *             parent,
                                  const std::string &   filename,
				  const std::string &   layoutAlgorithm );

    virtual YGraph * createGraph( YWidget *             parent,
                                  /* graph_t */ void *  graph );

    YQGraphPluginIf * impl;
};


#endif // YQGraphPluginStub_h
