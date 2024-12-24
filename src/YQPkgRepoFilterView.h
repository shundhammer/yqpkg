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


#ifndef YQPkgRepoFilterView_h
#define YQPkgRepoFilterView_h

#include "YQPkgSecondaryFilterView.h"

class QWidget;
class YQPkgRepoList;

class YQPkgRepoFilterView : public YQPkgSecondaryFilterView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgRepoFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgRepoFilterView();

    /**
     * Current selected repository, or 0 if nothing is selected
     **/
    zypp::Repository selectedRepo() const;


protected:

    virtual void primaryFilter();
    virtual void primaryFilterIfVisible();

    // Data members

    YQPkgRepoList * _repoList;
};



#endif // ifndef YQPkgRepoFilterView_h
