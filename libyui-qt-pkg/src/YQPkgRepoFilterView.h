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
  File:	      YQPkgRepoFilterView.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgRepoFilterView_h
#define YQPkgRepoFilterView_h

#include "YQPkgSecondaryFilterView.h"

class QWidget;
class YQPkgRepoList;

class YQPkgRepoFilterView : public YQPkgSecondaryFilterView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgRepoFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgRepoFilterView();

    /**
     * Current selected repository, or if nothing is selected
     */
    zypp::Repository selectedRepo() const;
    

protected:

    virtual void primaryFilter();
    virtual void primaryFilterIfVisible();

    // Data members
    // list of services, owned by the parent widget
    YQPkgRepoList *		_repoList;
};



#endif // ifndef YQPkgRepoFilterView_h
