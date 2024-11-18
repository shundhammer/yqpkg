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
  File:	      YQPkgDescriptionDialog.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgDescriptionDialog_h
#define YQPkgDescriptionDialog_h

#include <qdialog.h>


class YQPkgList;
class YQPkgDescriptionView;


/**
 * Pkg status and description as a standalone popup dialog.
 **/
class YQPkgDescriptionDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a description dialog for pkg 'pkgName'.
     **/
    static void showDescriptionDialog( const QString & pkgName );

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

    
protected:

    /**
     * Constructor: Creates a description dialog for all packages that match 'pkgName'.
     **/
    YQPkgDescriptionDialog( QWidget *		parent,
			    const QString &	pkgName );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that match the
     * specified package name.  
     **/
    void filter( const QString & pkgName );

    /**
     * Returns 'true' if the pkg list is empty.
     * This is only meaningful after calling 'filter()' !
     **/
    bool isEmpty() const;


    // Data members

    YQPkgList *			_pkgList;
    YQPkgDescriptionView *	_pkgDescription;
};


#endif // ifndef YQPkgDescriptionDialog_h
