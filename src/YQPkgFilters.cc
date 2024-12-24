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

#include <zypp/ResPool.h>
#include <zypp/PoolItem.h>

#include "Logger.h"
#include "YQPkgFilters.h"


ZyppProduct
YQPkgFilters::singleProductFilter(std::function<bool( const zypp::PoolItem & item)> filter )
{
    ZyppProduct product;

    // Thanks a lot for those 'auto' variables. That leaves figuring out WTH
    // this code does to the reader. This is the lazy Ruby attitude.

    auto product_begin = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product );
    auto product_end   = zypp::ResPool::instance().byKindEnd  ( zypp::ResKind::product );

    // Find the first product

    auto it = std::find_if( product_begin, product_end,
                            [&]( const zypp::PoolItem & item ) { return filter(item); }
                            );

    if ( it == product_end )
    {
        // logDebug() << "No product found " << endl;
        return product;
    }

    product = zypp::asKind<zypp::Product>( it->resolvable() );

    // Check if there is another product

    it = std::find_if( ++it, product_end,
                       [&](const zypp::PoolItem& item) { return filter(item); }
                       );

    if ( it == product_end )
    {
        logInfo() << "Found single product " << product->name() << endl;

        return product;
    }

    product = zypp::asKind<zypp::Product>( it->resolvable() );
    // logVerbose() << "Found another product " << product->name() << endl;

    return ZyppProduct(); // NULL pointer
}
