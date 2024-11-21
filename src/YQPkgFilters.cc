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


#include <algorithm>

#include "Logger.h"

#include <zypp/ResPool.h>
#include <zypp/PoolItem.h>

#include "YQPkgFilters.h"


ZyppProduct
YQPkgFilters::singleProductFilter(std::function<bool(const zypp::PoolItem& item)> filter)
{
    ZyppProduct product;

    auto it = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product );
    auto end = zypp::ResPool::instance().byKindEnd( zypp::ResKind::product );

    // Find the first product
    auto product_it = std::find_if(it, end, [&](const zypp::PoolItem& item) {
        return filter(item);
    });

    if (product_it == end)
    {
        logInfo() << "No product found " << endl;
        return product;
    }

    product = zypp::asKind<zypp::Product>( product_it->resolvable() );
    logInfo() << "Found product " << product->name() << endl;

    // Check if there is another product
    product_it = std::find_if(++product_it, end, [&](const zypp::PoolItem& item) {
        return filter(item);
    });

    if (product_it == end)
        return product;

    product = zypp::asKind<zypp::Product>( product_it->resolvable() );
    logInfo() << "Found another product " << product->name() << endl;

    // nullptr
    return ZyppProduct();
}
