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


#include "Logger.h"

#include "YQi18n.h"
#include "utf8.h"

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


