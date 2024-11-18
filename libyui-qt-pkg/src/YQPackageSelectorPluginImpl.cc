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
  File:		YQPackageSelectorPluginImpl.cc
  Author:	Stefan Hundhammer

*/


#include "YQPackageSelector.h"
#include "YQPatternSelector.h"
#include "YQSimplePatchSelector.h"
#include "YQPackageSelectorPluginImpl.h"

#include "Logger.h"
#include "Exception.h"


using std::endl;

//	Create YQPackageSelectorStart which reads the layout
//      term of the package selection dialog, creates the widget
//	tree and creates the YQPackageSelector.

extern "C"
{
    // Important to locate this symbol - see YQPackageSelectorPluginStub.cc in package yast2-qt
    YQPackageSelectorPluginImpl PSP;
}

YPackageSelector * YQPackageSelectorPluginImpl::createPackageSelector( YWidget * parent,
                                                                       long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQPackageSelector * packageSelector = 0;

    try
    {
        packageSelector = new YQPackageSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        logError() << "Caught std::exception: " << e.what() << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        logError() << "Caught unspecified exception." << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    CHECK_NEW( packageSelector );

    return packageSelector;
}


YWidget *YQPackageSelectorPluginImpl::createPatternSelector( YWidget * parent, long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQPatternSelector * patternSelector = 0;

    try
    {
        patternSelector = new YQPatternSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        logError() << "Caught std::exception: " << e.what() << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        logError() << "Caught unspecified exception." << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    CHECK_NEW( patternSelector );

    return patternSelector;

}

YWidget *YQPackageSelectorPluginImpl::createSimplePatchSelector( YWidget * parent, long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQSimplePatchSelector * simplePatchSelector = 0;

    try
    {
        simplePatchSelector = new YQSimplePatchSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        logError() << "Caught std::exception: " << e.what() << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        logError() << "Caught unspecified exception." << endl;
        logError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    CHECK_NEW( simplePatchSelector );

    return simplePatchSelector;

}
