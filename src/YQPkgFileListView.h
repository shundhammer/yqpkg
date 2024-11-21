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


#ifndef YQPkgFileListView_h
#define YQPkgFileListView_h

#include "YQPkgGenericDetailsView.h"


using std::list;
using std::string;


/**
 * @short Display a pkg's file list
 **/
class YQPkgFileListView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgFileListView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgFileListView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

protected:

    /**
     * Format a file list in HTML
     **/
    QString formatFileList( const list<string> & fileList ) const;
};


#endif // ifndef YQPkgFileListView_h
