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


/*
  Textdomain "qt-pkg"
*/


#include "Logger.h"

#include <yui/YUIException.h>

#include "YQPkgServiceFilterView.h"
#include "YQPkgServiceList.h"
#include "YQZypp.h"


YQPkgServiceFilterView::YQPkgServiceFilterView( QWidget * parent )
    : YQPkgSecondaryFilterView( parent )
{
    _serviceList = new YQPkgServiceList( this );
    CHECK_NEW( _serviceList );
    init(_serviceList);
}

YQPkgServiceFilterView::~YQPkgServiceFilterView()
{
    // NOP
}

void YQPkgServiceFilterView::primaryFilter()
{
    _serviceList->filter();
}

void YQPkgServiceFilterView::primaryFilterIfVisible()
{
    _serviceList->filterIfVisible();
}

// check if a libzypp service is present
bool YQPkgServiceFilterView::any_service()
{
	bool ret = std::any_of(ZyppRepositoriesBegin(), ZyppRepositoriesEnd(), [&](const zypp::Repository& repo) {
		// if the repository does not belong to any service then the service name is empty
		return !repo.info().service().empty();
	});

	logInfo() << "Found a libzypp service: " << ret << std::endl;
	return ret;
}

