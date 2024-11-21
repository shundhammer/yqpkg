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


#ifndef YQPkgSelDescriptionView_h
#define YQPkgSelDescriptionView_h

#include "YQPkgDescriptionView.h"


/**
 * @short Display the description of a zypp::ResObject derived object along with its
 * name and summary.
 **/
class YQPkgSelDescriptionView : public YQPkgDescriptionView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelDescriptionView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelDescriptionView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

protected:

    /**
     * Format the heading in HTML
     **/
    QString htmlHeading( ZyppSel selectable );

    /**
     * Check if 'icon' exists. Returns 'icon' if it exists and an empty string
     * if it doesn't exist. 
     **/
    QString findIcon( const QString & icon ) const;
};


#endif // ifndef YQPkgSelDescriptionView_h
