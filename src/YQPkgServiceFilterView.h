/*  ------------------------------------------------------
              __   _____  ____  _         
              \ \ / / _ \|  _ \| | ____ _ 
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
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
