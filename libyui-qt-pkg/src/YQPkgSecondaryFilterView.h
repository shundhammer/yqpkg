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


#ifndef YQPkgSecondaryFilterView_h
#define YQPkgSecondaryFilterView_h

#include "YQZypp.h"
#include <QWidget>

class QY2ComboTabWidget;
class YQPkgSearchFilterView;
class YQPkgStatusFilterView;


/**
 * This is a base class for filter views containing a secondary filter
 */
class YQPkgSecondaryFilterView : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * Because of the circular dependencies you have to call the init()
     * method later with the new subclassed object.
     **/
    YQPkgSecondaryFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSecondaryFilterView();

    /**
     * Initialize the primary widget
     * @param primary_widget the primary widget to which the secondary views
     *      will be added. It should be a subclass of YQPkgSecondaryFilterView.
     */
    void init(QWidget * primary_widget);

signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package comes from the respective repository
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package does not come from the respective repository
     **/
    void filterNearMatch( ZyppSel	selectable,
			  ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();

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


protected slots:

    /**
     * Propagate a filter match from the primary filter
     * and appy any selected secondary filter(s) to it
     **/
    void primaryFilterMatch( ZyppSel	selectable,
			     ZyppPkg 	pkg );

    /**
     * Propagate a filter near match from the primary filter
     * and appy any selected secondary filter(s) to it
     **/
    void primaryFilterNearMatch( ZyppSel	selectable,
				 ZyppPkg	pkg );

protected:

    /**
     * Widget layout for the secondary filters
     **/
    QWidget * layoutSecondaryFilters( QWidget * parent, QWidget * primary_widget );

    /**
     * Check if pkg matches the the currently selected secondary filter
     **/
    bool secondaryFilterMatch( ZyppSel 	selectable,
			       ZyppPkg 		pkg );

    virtual void primaryFilter() {}
    virtual void primaryFilterIfVisible() {}


    // Data members
    QY2ComboTabWidget *		_secondaryFilters;
    QWidget *			    _allPackages;
    QWidget *_unmaintainedPackages;
    YQPkgSearchFilterView *	    _searchFilterView;
    YQPkgStatusFilterView *	    _statusFilterView;
};

#endif // ifndef YQPkgSecondaryFilterView_h
