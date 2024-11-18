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
  File:		YQPkgFileListView.cc
  Author:	Stefan Hundhammer <shundhammer@suse.com>

  Textdomain	"qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <QRegExp>

#include "YQPkgFileListView.h"
#include "YQPkgDescriptionDialog.h"


#define MAX_LINES 500


YQPkgFileListView::YQPkgFileListView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgFileListView::~YQPkgFileListView()
{
    // NOP
}


void
YQPkgFileListView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html = htmlHeading( selectable,
				false ); // showVersion

    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( installed )
    {
        // ma@: It might be worth passing Package::FileList directly
        // instead of copying _all_ filenames into a list first.
        // Package::FileList is a query, so it does not eat much memory.
        zypp::Package::FileList f( installed->filelist() );
        std::list<std::string> tmp( f.begin(), f.end() );
	html += formatFileList( tmp );
    }
    else
    {
	html += "<p><i>" + _( "Information only available for installed packages." ) + "</i></p>";
    }

    setHtml( html );
}



QString YQPkgFileListView::formatFileList( const list<string> & fileList ) const
{
    QString html;
    unsigned line_count = 0;

    for ( list<string>::const_iterator it = fileList.begin();
	  it != fileList.end() && line_count < MAX_LINES;
	  ++it, ++line_count )
    {
	QString line = htmlEscape( fromUTF8( *it ) );

	if ( line.contains( "/bin/"  ) ||
	     line.contains( "/sbin/" )	 )
	{
	    line = "<b>" + line + "</b>";
	}

	html += line + "<br>";
    }

    if ( fileList.size() > MAX_LINES )
    {
	html += "...<br>";
	html += "...<br>";
    }
    else
    {
        // %1 is the total number of files in a file list
        html += "<br>" + _( "%1 files total" ).arg( (unsigned long) fileList.size() );
    }

    return "<p>" + html + "</p>";
}


