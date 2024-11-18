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

  File:		YPackageSelector.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YPackageSelector_h
#define YPackageSelector_h

#include "YWidget.h"

#define YPkg_TestMode		1 << 0	// Test mode for debugging
#define YPkg_OnlineUpdateMode	1 << 1	// Online update mode: Show patches
#define YPkg_UpdateMode		1 << 2	// Start with "Update problems" filter view
#define YPkg_SearchMode		1 << 3	// Start with "Search"  filter view
#define YPkg_SummaryMode	1 << 4	// Start with "Summary" filter view
#define YPkg_RepoMode		1 << 5	// Start with "Repositories" filter view
#define YPkg_RepoMgr		1 << 6	// Add "Manage Repositories" to menu
#define YPkg_ConfirmUnsupported	1 << 7	// Confirm unsupported packages
#define YPkg_OnlineSearch		1 << 8	// Add "Search Online" to menu

/**
 * A simple wrapper for an insanely complex UI for installing software.
 **/
class YPackageSelector : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * 'modeFlags' are flags determining which modes to use, ORed together:
     *     YPkg_OnlineUpdateMode | YPkg_TestMode
     **/
    YPackageSelector( YWidget * parent, long modeFlags = 0 );

public:
    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YPackageSelector"; }

    /**
     * Check for the various modes.
     **/
    bool testMode()		const { return _modeFlags & YPkg_TestMode;	   	}
    bool onlineUpdateMode()	const { return _modeFlags & YPkg_OnlineUpdateMode; 	}
    bool updateMode()		const { return _modeFlags & YPkg_UpdateMode;	   	}
    bool searchMode() 		const { return _modeFlags & YPkg_SearchMode;	   	}
    bool summaryMode()		const { return _modeFlags & YPkg_SummaryMode;	   	}
    bool repoMode()		const { return _modeFlags & YPkg_RepoMode;         	}
    bool repoMgrEnabled()	const { return _modeFlags & YPkg_RepoMgr;		}
    bool confirmUnsupported()	const { return _modeFlags & YPkg_ConfirmUnsupported;	}
    bool onlineSearchEnabled()	const { return _modeFlags & YPkg_OnlineSearch;		}

protected:
    long _modeFlags;
};


#endif // YPackageSelector_h
