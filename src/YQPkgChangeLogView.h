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


#ifndef YQPkgChangeLogView_h
#define YQPkgChangeLogView_h

#include <zypp/Changelog.h>
#include "YQPkgGenericDetailsView.h"


using std::list;
using std::string;


/**
 * Display a pkg's file list
 **/
class YQPkgChangeLogView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgChangeLogView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgChangeLogView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

protected:

    /**
     * Format a change log list in HTML
     **/
    QString changeLogTable( const zypp::Changelog & changeLog ) const;

    /**
     * Format an info message about not displayed changes.
     * @param  missing Number of missing entris
     * @param  pkg     Package name (with version)
     * @return         Translated message
     */
    QString notDisplayedChanges(int missing, const std::string &pkg);
};


#endif // ifndef YQPkgChangeLogView_h
