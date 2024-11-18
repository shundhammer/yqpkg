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

  File:		YQPackageSelectorPluginStub.h

  Authors:	Katharina Machalkova <kmachalkova@suse.cz>
		Stephan Kulow <coolo@suse.de>

/-*/


#ifndef YQPackageSelectorPluginStub_h
#define YQPackageSelectorPluginStub_h

#include <yui/YPackageSelectorPlugin.h>
#include <yui/YDialog.h>
#include <yui/YEvent.h>

#include "YQPackageSelectorPluginIf.h"


/**
 * Simplified access to the package selector plugin.
 **/
class YQPackageSelectorPluginIf;


class YQPackageSelectorPluginStub: public YPackageSelectorPlugin
{
public:
    /**
     * Constructor: Load the plugin library for the package selector.
     **/
    YQPackageSelectorPluginStub();

    /**
     * Destructor.
     **/
    virtual ~YQPackageSelectorPluginStub();

    /**
     * Create a package selector.
     * Implemented from YPackageSelectorPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YPackageSelector * createPackageSelector( YWidget *	parent,
						      long	modeFlags );

    /**
     * Create a pattern selector (optional widget).
     **/
    virtual YWidget * createPatternSelector( YWidget *	parent,
                                             long	modeFlags );

    /**
     * Create a simple patch selector (optional widget).
     **/
    virtual YWidget * createSimplePatchSelector( YWidget * parent,
                                                 long	   modeFlags );


    YQPackageSelectorPluginIf * impl;
};


#endif // YQPackageSelectorPluginStub_h
