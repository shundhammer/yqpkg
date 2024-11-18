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
  File:	      YQPkgProductList.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include "Logger.h"

#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include "YQPkgProductList.h"


using std::list;
using std::set;


YQPkgProductList::YQPkgProductList( QWidget * parent )
    : YQPkgObjList( parent )
    , _vendorCol( -42 )
{
    logDebug() << "Creating product list" << endl;

    QStringList headers;
    int numCol = 0;
    headers << ( "" );			_statusCol	= numCol++;
    headers << _( "Product"	);	_nameCol	= numCol++;
    headers <<  _( "Summary"	);	_summaryCol	= numCol++;
    headers <<  _( "Version" 	);	_versionCol	= numCol++;
    headers <<  _( "Vendor" 	);	_vendorCol	= numCol++;

    setColumnCount( numCol );
    setHeaderLabels(headers);

    setAllColumnsShowFocus( true );
    //setColumnAlignment( sizeCol(), Qt::AlignRight );

    setSortingEnabled( true );
    sortByColumn( nameCol(), Qt::AscendingOrder );



    fillList();
    selectSomething();

    logDebug() << "Creating product list done" << endl;
}


YQPkgProductList::~YQPkgProductList()
{
    // NOP
}


void
YQPkgProductList::fillList()
{
    clear();
    logDebug() << "Filling product list" << endl;

    for ( ZyppPoolIterator it = zyppProductsBegin();
	  it != zyppProductsEnd();
	  ++it )
    {
	ZyppProduct zyppProduct = tryCastToZyppProduct( (*it)->theObj() );

	if ( zyppProduct )
	{
	    addProductItem( *it, zyppProduct );
	}
	else
	{
	    logError() << "Found non-product selectable" << endl;
	}
    }

    logDebug() << "product list filled" << endl;
    resizeColumnToContents(_statusCol);
}


void
YQPkgProductList::addProductItem( ZyppSel	selectable,
				  ZyppProduct	zyppProduct )
{
    if ( ! selectable )
    {
	logError() << "NULL ZyppSel!" << endl;
	return;
    }

    new YQPkgProductListItem( this, selectable, zyppProduct );
}






YQPkgProductListItem::YQPkgProductListItem( YQPkgProductList * 	productList,
					    ZyppSel		selectable,
					    ZyppProduct 	zyppProduct )
    : YQPkgObjListItem( productList, selectable, zyppProduct )
    , _productList( productList )
    , _zyppProduct( zyppProduct )
{
    if ( ! _zyppProduct )
	_zyppProduct = tryCastToZyppProduct( selectable->theObj() );

    if ( ! _zyppProduct )
	return;

    setStatusIcon();

    if ( vendorCol() > -1 )
	setText( vendorCol(), zyppProduct->vendor() );

}


YQPkgProductListItem::~YQPkgProductListItem()
{
    // NOP
}




void
YQPkgProductListItem::applyChanges()
{
    solveResolvableCollections();
}



