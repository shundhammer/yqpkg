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


#ifndef YQPkgFilters_h
#define YQPkgFilters_h

#include "YQZypp.h"

/**
 * @short Zypp filtering helpers
 **/
class YQPkgFilters
{

public:

    /**
     * Returns the product if the filter finds a single product
     * or null product if there are no or multiple products.
      * @param  filter filtering function, the parameter is the product pool item,
      *     returning true if it matches the expectations
      * @return      Found zypp product or null
      */
    static ZyppProduct singleProductFilter( std::function<bool(const zypp::PoolItem& item)> filter);

};

#endif // ifndef YQPkgFilters_h
