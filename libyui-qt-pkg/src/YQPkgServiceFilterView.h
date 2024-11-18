/*
  Copyright (c) 2018-2021 SUSE LLC

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


#ifndef YQPkgServiceFilterView_h
#define YQPkgServiceFilterView_h

#include "YQPkgSecondaryFilterView.h"

class QWidget;
class YQPkgServiceList;

/**
 * A widget to display a libzypp servic filter view. It should be used only when
 * a libzypp service is present in the system.
 */
class YQPkgServiceFilterView : public YQPkgSecondaryFilterView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgServiceFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgServiceFilterView();

    /**
     * Is any enabled libzypp service present?
     */
    static bool any_service();

protected:

    virtual void primaryFilter();
    virtual void primaryFilterIfVisible();

    // Data members
    // list of services, owned by the parent widget
    YQPkgServiceList *		_serviceList;
};

#endif // ifndef YQPkgServiceFilterView_h
