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
  File:	      YQPkgProductDialog.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgProductDialog_h
#define YQPkgProductDialog_h

#include <QDialog>


class QTabWidget;
class YQPkgProductList;
class YQPkgDependenciesView;


/**
 * Products list with dependencies details view
 **/
class YQPkgProductDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a dialog with all products
     **/
    static void showProductDialog( QWidget* parent = 0);

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

    /**
     * Delayed initialization after the dialog is fully created.
     *
     * Reimplemented from QWidget.
     **/
    virtual void polish();


protected:

    /**
     * Constructor: Creates a description dialog for all packages that match 'pkgName'.
     **/
    YQPkgProductDialog( QWidget * parent );


    // Data members

    YQPkgProductList *		_productList;
    QTabWidget *		_detailsViews;
    YQPkgDependenciesView *	    _dependenciesView;
};


#endif // ifndef YQPkgProductDialog_h
