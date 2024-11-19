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
  File:	      YQPkgTechnicalDetailsView.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include "Logger.h"

#include "YQi18n.h"
#include <yui/qt/utf8.h>

#include "YQPkgTechnicalDetailsView.h"


using std::list;
using std::string;


YQPkgTechnicalDetailsView::YQPkgTechnicalDetailsView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgTechnicalDetailsView::~YQPkgTechnicalDetailsView()
{
    // NOP
}


void
YQPkgTechnicalDetailsView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }



    QString html_text = htmlStart();
    
    html_text += htmlHeading( selectable );

    ZyppPkg candidate = tryCastToZyppPkg( selectable->candidateObj() );
    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( candidate && installed && candidate != installed )
    {
	html_text += complexTable( selectable, installed, candidate );
    }
    else
    {
	if ( candidate )
	    html_text += simpleTable( selectable, candidate );

	if ( installed )
	    html_text += simpleTable( selectable, installed );
    }

    html_text += htmlEnd();

    setHtml( html_text );
}


QString
YQPkgTechnicalDetailsView::authorsListCell( ZyppPkg pkg ) const
{
    QString html = "<td align='top'>";
    QString line;
    list<string> authors = pkg->authors();
    list<string>::const_iterator it = authors.begin();

    while ( it != authors.end() )
    {
	line = fromUTF8( *it );
	line = htmlEscape( line );
	html += line + "<br>";
	++it;
    }

    html += "</td>";

    return html;
}


QString
YQPkgTechnicalDetailsView::simpleTable( ZyppSel selectable,
					ZyppPkg		pkg )
{
    QString html;

    html += row( hcell( _( "Version:"		) ) + cell( pkg->edition().asString()		) );
    html += row( hcell( _( "Build Time:"	) ) + cell( pkg->buildtime()			) );

    html +=
	*pkg == selectable->installedObj() ?
	row( hcell( _( "Install Time:" ) ) + cell( pkg->installtime() ) ) : "";

    html += row( hcell( _( "License:"		) ) + cell( pkg->license()			) );
    html += row( hcell( _( "Installed Size:" 	) ) + cell( pkg->installSize().asString()	) );
    html += row( hcell( _( "Download Size:"  	) ) + cell( pkg->downloadSize().asString()	) );
    html += row( hcell( _( "Distribution:"	) ) + cell( pkg->distribution()			) );
    html += row( hcell( _( "Vendor:"		) ) + cell( pkg->vendor()			) );
    html += row( hcell( _( "Packager:"		) ) + cell( pkg->packager()			) );
    html += row( hcell( _( "Architecture:"	) ) + cell( pkg->arch().asString()		) );
    html += row( hcell( _( "Build Host:"	) ) + cell( pkg->buildhost()			) );
    html += row( hcell( _( "URL:"		) ) + cell( pkg->url()				) );
    html += row( hcell( _( "Source Package:"    ) ) + cell( pkg->sourcePkgName() + "-" + pkg->sourcePkgEdition().asString() ) );
    html += row( hcell( _( "Media No.:"		) ) + cell( pkg->mediaNr()			) );
    html += row( hcell( _( "Authors:"		) ) + authorsListCell( pkg			) );
    
    html = "<br>" + table( html );

    return html;
}


QString
YQPkgTechnicalDetailsView::complexTable( ZyppSel	selectable,
					 ZyppPkg	installed,
					 ZyppPkg	candidate )
{
    ZyppPkg p1 = candidate;
    ZyppPkg p2 = installed;

    QString p1_header = _( "<b>Alternate Version</b>" );
    QString p2_header = _( "<b>Installed Version</b>" );

    QString html;

    html += row( hcell( QString( "" ) )		    + hcell( "<b>" + p1_header + "</b>"	    	) + hcell( "<b>" + p2_header + "</b>"	) );

    html += row( hcell( _( "Version:"		) ) + cell( p1->edition().asString()		) + cell( p2->edition().asString()	) );
    html += row( hcell( _( "Build Time:"	) ) + cell( p1->buildtime()			) + cell( p2->buildtime()		) );
    html += row( hcell( _( "Install Time:"	) ) + cell( p1->installtime()			) + cell( p2->installtime()		) );
    html += row( hcell( _( "License:"		) ) + cell( p1->license()			) + cell( p2->license()			) );
    html += row( hcell( _( "Installed Size:" 	) ) + cell( p1->installSize().asString()	) + cell( p2->installSize().asString()	) );
    html += row( hcell( _( "Download Size:"  	) ) + cell( p1->downloadSize().asString()	) + cell( p2->downloadSize().asString()	) );
    html += row( hcell( _( "Distribution:"	) ) + cell( p1->distribution()			) + cell( p2->distribution()		) );
    html += row( hcell( _( "Vendor:"		) ) + cell( p1->vendor()			) + cell( p2->vendor()			) );
    html += row( hcell( _( "Packager:"		) ) + cell( p1->packager()			) + cell( p2->packager()		) );
    html += row( hcell( _( "Architecture:"	) ) + cell( p1->arch().asString()		) + cell( p2->arch().asString()		) );
    html += row( hcell( _( "Build Host:"	) ) + cell( p1->buildhost()			) + cell( p2->buildhost()		) );
    html += row( hcell( _( "URL:"		) ) + cell( p1->url()				) + cell( p2->url()			) );
    html += row( hcell( _( "Source Package:"	) ) + cell( p1->sourcePkgName() + "-" + p1->sourcePkgEdition().asString() )	
						    + cell( p2->sourcePkgName() + "-" + p2->sourcePkgEdition().asString()		) );
    html += row( hcell( _( "Media No.:"		) ) + cell( p1->mediaNr()			) + cell( p2->mediaNr()			) );
    html += row( hcell( _( "Authors:"		) ) + authorsListCell( p1			) + authorsListCell( p2			) );


    html = "<br>" + table( html );
    
    return html;
}


