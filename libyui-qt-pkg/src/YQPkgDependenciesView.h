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
  File:	      YQPkgDependenciesView.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgDependenciesView_h
#define YQPkgDependenciesView_h

#include "YQZypp.h"
#include "YQPkgGenericDetailsView.h"

using std::string;


/**
 * @short Display technical details ( very much like 'rpm -qi' ) for a zypp::Package
 * object - the installed instance, the candidate instance or both ( in two
 * columns ) if both exist. All other available instances are ignored.
 **/
class YQPkgDependenciesView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgDependenciesView( QWidget * parent );


    /**
     * Destructor
     **/
    virtual ~YQPkgDependenciesView();


protected:

    /**
     * Show details for the specified selectable:
     * In this case technical data, very much like "rpm -qi".
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

    /**
     * Returns a string containing a HTML table for technical details for one
     * package.
     **/
    QString simpleTable( ZyppObj pkg );


    /**
     * Returns a string containing a HTML table for technical details for two
     * package instances: The installed instance and an alternate instance.
     * (usually the candidate instance).
     **/
    QString complexTable( ZyppObj installed,
			  ZyppObj candidate );

    /**
     * Format a zypp::CapSet (describing zypp::Dep::REQUIRES etc.)
     * with a heading in HTML lines.
     *
     * Returns an empty string if capSet is empty.
     **/
    static QString row( const QString &		heading,
			const zypp::Capabilities & 	capSet );


    /**
     * Format two zypp::Capabilities (describing zypp::Dep::REQUIRES etc.)
     * with a heading in HTML lines.
     *
     * Returns an empty string both capSets are empty.
     **/
    static QString row( const QString & 	heading,
			const zypp::Capabilities & 	capSet1,
			const zypp::Capabilities & 	capSet2 );

    /**
     * Returns a string containing a HTML table row with 'contents'.
     *
     * Required here to avoid inheritance ambiguities.
     **/
    static QString row( const QString & contents )
	{ return YQPkgGenericDetailsView::row( contents ); }

    /**
     * Format a zypp::Capabilities (describing zypp::Dep::REQUIRES etc.)
     * in HTML lines, separated with <BR>.
     * Returns an empty string if capSet is empty.
     **/
    static QString htmlLines( const zypp::Capabilities & capSet );
};


#endif // ifndef YQPkgDependenciesView_h
