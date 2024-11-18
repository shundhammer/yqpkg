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
  File:		YQPkgRepoFilterView.cc
  Author:	Stefan Hundhammer <shundhammer@suse.com>

  Textdomain	"qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

#include <yui/YUIException.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/QY2ComboTabWidget.h>

#include <QVBoxLayout>
#include <QSplitter>

#include "YQPkgRepoFilterView.h"
#include "YQPkgRepoList.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgStatusFilterView.h"
#include "YQZypp.h"
#include "QY2LayoutUtils.h"


YQPkgRepoFilterView::YQPkgRepoFilterView( QWidget * parent )
    : YQPkgSecondaryFilterView( parent )
{
    _repoList = new YQPkgRepoList( this );
    YUI_CHECK_NEW( _repoList );
    init(_repoList);
}


YQPkgRepoFilterView::~YQPkgRepoFilterView()
{
    // NOP
}

ZyppRepo
YQPkgRepoFilterView::selectedRepo() const
{
    YQPkgRepoListItem *selection = _repoList->selection();
    if ( selection && selection->zyppRepo() )
        return selection->zyppRepo();
    return zypp::Repository::noRepository;
}


void YQPkgRepoFilterView::primaryFilter()
{
    _repoList->filter();
}

void YQPkgRepoFilterView::primaryFilterIfVisible()
{
    _repoList->filterIfVisible();
}

