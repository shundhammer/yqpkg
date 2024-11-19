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
  File:	      YQPkgUpdateProblemFilterView.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgUpdateProblemFilterView_h
#define YQPkgUpdateProblemFilterView_h


#include "YQZypp.h"
#include <QTextBrowser>
#include "YQPkgSelMapper.h"


/**
 * @short Filter view for packages that made problems during update
 **/
class YQPkgUpdateProblemFilterView : public QTextBrowser
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgUpdateProblemFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgUpdateProblemFilterView();

    /**
     * Check if there are any problematic packages at all, i.e. if it is
     * worthwhile to create this widget at all
     **/
    static bool haveProblematicPackages();


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:

    YQPkgSelMapper _selMapper;
};



#endif // ifndef YQPkgUpdateProblemFilterView_h
