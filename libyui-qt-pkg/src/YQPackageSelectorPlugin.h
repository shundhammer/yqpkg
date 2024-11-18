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
  File:	      YQPackageSelectorPlugin.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPackageSelectorPlugin_h
#define YQPackageSelectorPlugin_h

#include <yui/YPackageSelectorPlugin.h>

#include "YQPackageSelector.h"
#include "YQPatternSelector.h"
#include "YQSimplePatchSelector.h"


/**
 * Simplified access to the Qt UI's package selector plugin.
 **/
class YQPackageSelectorPlugin: public YPackageSelectorPlugin
{
public:

    /**
     * Constructor: Load the plugin library for the Qt package selector.
     **/
    YQPackageSelectorPlugin();

    /**
     * Destructor. Calls dlclose() which will unload the plugin library if it
     * is no longer used, i.e. if the reference count dlopen() uses reaches 0.
     **/
    virtual ~YQPackageSelectorPlugin();

    /**
     * Create a package selector.
     * Implemented from YPackageSelectorPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib. 
     **/
    virtual YQPackageSelector * createPackageSelector	( YWidget * 	parent,
							  long		modeFlags = 0 );
    
    /**
     * Create a pattern selector.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib. 
     **/
    virtual YQPatternSelector * createPatternSelector	( YWidget * 	parent,
							  long		modeFlags = 0 );
    /**
     * Create a simple patch selector.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib. 
     **/
    virtual YQSimplePatchSelector * createSimplePatchSelector( YWidget * parent,
							       long	modeFlags = 0 );
};



#endif // YQPackageSelectorPlugin_h
