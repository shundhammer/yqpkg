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
  File:	      YQPkgProductList.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgProductList_h
#define YQPkgProductList_h

#include <string>
#include "YQPkgObjList.h"

using std::string;

class YQPkgProductListItem;


/**
 * @short Display a list of zypp::Product objects.
 **/
class YQPkgProductList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgProductList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgProductList();


public slots:

    /**
     * Add a product to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addProductItem( ZyppSel	selectable,
			 ZyppProduct	zyppProduct );

    /**
     * Fill the product list.
     **/
    void fillList();


public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgProductListItem * selection() const;

    /**
     * Returns the column for the product's vendor.
     **/
    int vendorCol() const { return _vendorCol; }


protected:


    // Data members

    int _vendorCol;
};



class YQPkgProductListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgProductList item that corresponds to
     * zyppProduct.
     **/
    YQPkgProductListItem( YQPkgProductList * 	productList,
			  ZyppSel		selectable,
			  ZyppProduct 		zyppProduct );

    /**
     * Destructor
     **/
    virtual ~YQPkgProductListItem();

    /**
     * Returns the original zyppProduct object.
     **/
    ZyppProduct zyppProduct() const { return _zyppProduct; }


    // Columns

    int statusCol()	const	{ return _productList->statusCol();	}
    int summaryCol()	const	{ return _productList->summaryCol();	}
    int versionCol()	const   { return _productList->versionCol();	}
    int vendorCol()	const   { return _productList->vendorCol();	}


protected:

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all products, patterns, etc.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgProductList *	_productList;
    ZyppProduct		_zyppProduct;
};


#endif // ifndef YQPkgProductList_h
