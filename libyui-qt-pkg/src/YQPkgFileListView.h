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
  File:	      YQPkgFileListView.h
  Author:     Stefan Hundhammer <shundhammer.de>
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
