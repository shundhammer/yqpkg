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
  File:	      YQSimplePatchSelector.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQSimplePatchSelector_h
#define YQSimplePatchSelector_h

#include <QColor>

#include "YQPackageSelectorBase.h"


class QPushButton;

class YQPkgPatchFilterView;
class YQPkgPatchList;


class YQSimplePatchSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQSimplePatchSelector( YWidget * parent, long modeFlags );


protected slots:

    /**
     * User clicked on "Details..." - start the detailed package selection.
     * This will return from UI::RunPkgSelection() with :details .
     **/
    void detailedPackageSelection();

    /**
     * Debugging
     **/
    void debugTrace();


protected:

    // Layout methods - create and layout widgets

    void	basicLayout();
    void	layoutButtons	( QWidget * parent );

    /**
     * Establish Qt signal / slot connections.
     **/
    void makeConnections();


    // Data members

    YQPkgPatchFilterView *	_patchFilterView;
    YQPkgPatchList *		_patchList;
};



#endif // YQSimplePatchSelector_h
