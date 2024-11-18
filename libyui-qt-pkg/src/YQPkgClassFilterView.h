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
  File:		YQPkgClassFilterView.h

  Authors:	Duncan Mac-Vicar Prett <duncan@suse.de>
		Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgClassFilterView_h
#define YQPkgClassFilterView_h

#include "YQZypp.h"
#include <QTreeWidget>


typedef enum
{
    YQPkgClassNone,             // Not listed in the widget
    YQPkgClassSuggested,
    YQPkgClassRecommended,
    YQPkgClassOrphaned,
    YQPkgClassUnneeded,
    YQPkgClassMultiversion,
    YQPkgClassRetracted,
    YQPkgClassRetractedInstalled,
    YQPkgClassAll,
} YQPkgClass;


class YQPkgClassItem;


/**
 * Filter view for package classes (categories) like suggested, recommended,
 * orphaned etc. packages. See enum YPkgClass.
 **/
class YQPkgClassFilterView : public QTreeWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgClassFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgClassFilterView();

    /**
     * Check if 'pkg' matches the selected package class and send a filterMatch
     * signal if it does.
     *
     * Returns 'true' if there is a match, 'false' otherwise.
     **/
    bool check( ZyppSel	selectable, ZyppPkg pkg );

    /**
     * Check if 'pkg' matches the selected package class.
     * Returns 'true' if there is a match, 'false' otherwise.
     **/
    bool checkMatch( ZyppSel selectable, ZyppPkg pkg );

    /**
     * Returns the currently selected YQPkgClass
     **/
    YQPkgClass selectedPkgClass() const;

    /**
     * Show the specified package class, i.e. select that filter.
     **/
    void showPkgClass( YQPkgClass pkgClass );


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *	  filterStart()
     *	  filterMatch() for each pkg that matches the filter
     *	  filterFinished()
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
    void filterMatch( ZyppSel selectable, ZyppPkg pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    void slotSelectionChanged( QTreeWidgetItem * newSelection );


protected:

    void fillPkgClasses();

};


class YQPkgClassItem: public QTreeWidgetItem
{
public:

    YQPkgClassItem( YQPkgClassFilterView * parentFilterView,
		    YQPkgClass             pkgClass );

    virtual ~YQPkgClassItem();

    YQPkgClass pkgClass() const { return _pkgClass; }

    virtual bool operator< ( const QTreeWidgetItem & otherListViewItem ) const;


private:

    // Data members

    YQPkgClass _pkgClass;
};


#endif // ifndef YQPkgClassFilterView_h
