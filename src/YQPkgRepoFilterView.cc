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



#include "Exception.h"
#include "Logger.h"
#include "YQPkgRepoList.h"
#include "YQZypp.h"
#include "YQi18n.h"
#include "YQPkgRepoFilterView.h"


YQPkgRepoFilterView::YQPkgRepoFilterView( QWidget * parent )
    : YQPkgSecondaryFilterView( parent )
{
    _repoList = new YQPkgRepoList( this );
    CHECK_NEW( _repoList );

    init( _repoList) ;
}


YQPkgRepoFilterView::~YQPkgRepoFilterView()
{
    // NOP
}


ZyppRepo
YQPkgRepoFilterView::selectedRepo() const
{
    YQPkgRepoListItem * selection = _repoList->selection();

    if ( selection && selection->zyppRepo() )
        return selection->zyppRepo();

    return zypp::Repository::noRepository;
}


void YQPkgRepoFilterView::primaryFilter()
{
    logVerbose() << "filtering" << endl;

    _repoList->filter();
}
