/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
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
     * Notification that a new filter is the one to be shown.
     **/
    void showFilter( QWidget * newFilter );

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();


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
