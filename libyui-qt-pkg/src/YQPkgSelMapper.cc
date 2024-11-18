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
  File:	      YQPkgSelMapper.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#include "Logger.h"

#include "YQPkgSelMapper.h"


using std::endl;

int			YQPkgSelMapper::_refCount = 0;
YQPkgSelMapper::Cache	YQPkgSelMapper::_cache;


YQPkgSelMapper::YQPkgSelMapper()
{
    if ( ++_refCount == 1 )
	rebuildCache();
}


YQPkgSelMapper::~YQPkgSelMapper()
{
    if ( --_refCount == 0 )
    {
	logDebug() << "Destroying pkg -> selectable cache"  << endl;
	_cache.clear();
    }
}


void YQPkgSelMapper::rebuildCache()
{
    _cache.clear();
    logDebug() << "Building pkg -> selectable cache" << endl;

    for ( ZyppPoolIterator sel_it = zyppPkgBegin();
	  sel_it != zyppPkgEnd();
	  ++sel_it )
    {
	ZyppSel sel = *sel_it;

	if ( sel->installedObj() )
	{
	    // The installed package (if there is any) may or may not be in the list
	    // of available packages. Better make sure to insert it.

	    ZyppPkg installedPkg = tryCastToZyppPkg( sel->installedObj() );

	    if ( installedPkg )
		_cache.insert( CachePair( installedPkg, sel ) );
	}

	zypp::ui::Selectable::available_iterator it = sel->availableBegin();

	while ( it != sel->availableEnd() )
	{
	    ZyppPkg pkg = tryCastToZyppPkg( *it );

	    if ( pkg )
		_cache.insert( CachePair( pkg, sel ) );

	    ++it;
	}
    }

    logDebug() << "Building pkg -> selectable cache done" << endl;
}


ZyppSel
YQPkgSelMapper::findZyppSel( ZyppPkg pkg )
{
    YQPkgSelMapper mapper; // This will build a cache, if there is none yet
    ZyppSel sel;

    YQPkgSelMapper::CacheIterator it = YQPkgSelMapper::_cache.find( pkg );

    if ( it != YQPkgSelMapper::_cache.end() )
	sel = it->second;
    else
	logWarning() << "No selectable found for package " << pkg->name() << endl;

    return sel;
}

