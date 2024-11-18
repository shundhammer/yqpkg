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
  File:	      YQPkgSelDescriptionView.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#include "Logger.h"

#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <unistd.h>

#include <QRegExp>
#include <qbuffer.h>

#include "YQPkgSelDescriptionView.h"


using std::endl;
using std::list;
using std::string;


YQPkgSelDescriptionView::YQPkgSelDescriptionView( QWidget * parent )
    : YQPkgDescriptionView( parent )
{
}


YQPkgSelDescriptionView::~YQPkgSelDescriptionView()
{
    // NOP
}


void
YQPkgSelDescriptionView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( selectable );

    QString description = fromUTF8( selectable->theObj()->description() );

    if ( ! description.contains( "<!-- DT:Rich -->" ) )
	description = simpleHtmlParagraphs( description );

    html_text += description;

    setHtml( html_text );
    //FIXME ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
}



QString
YQPkgSelDescriptionView::htmlHeading( ZyppSel selectable )
{
    ZyppObj       	zyppObj       	= selectable->theObj();
    ZyppPattern		pattern		= tryCastToZyppPattern  ( zyppObj );

    if ( ! pattern )
	return YQPkgGenericDetailsView::htmlHeading( selectable );

    QString summary = fromUTF8( zyppObj->summary() );
    bool useBigFont = ( summary.length() <= 40 );

    if ( summary.isEmpty() )			// No summary?
	summary = fromUTF8( zyppObj->name() );	// Use name instead (internal only normally)

    QString iconName = pattern ? pattern->icon().asString().c_str() : "";

    if ( iconName.isEmpty() )
    {
	iconName = zyppObj->name().c_str();
	iconName.replace( ' ', '_' );
    }

    if ( ! iconName.isEmpty() )
    {
	if ( iconName.startsWith( "./" ) )
	    iconName.replace( QRegExp( "^\\./" ), "" );

	if ( pattern && iconName.isEmpty() )
	    logWarning() << "No icon for pattern " << zyppObj->name() << endl;
    }


    QString html = "<table width='100%'><tr><td>"
	+ ( useBigFont ? QString( "<h2>" ) : QString( "<b>" ) )
	+ summary
	+ ( useBigFont ? QString( "</h2>" ) : QString( "</b>" ) )
	+ "</td></tr>"
	+ "</table>";

    if ( ! iconName.isEmpty() )
    {
	QIcon icon = YQUI::ui()->loadIcon( iconName.toStdString() );
        QPixmap pixmap = icon.pixmap( useBigFont ? 32 : 16 );
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        pixmap.save(&buffer, "PNG");
        // The white-space: nowrap is to work around https://bugreports.qt.io/browse/QTBUG-86671
	html = QString( "<table width='100%'><tr>" )
	    + "<td style='white-space: nowrap;' valign='middle'><img src=\"data:image/png;base64," + byteArray.toBase64() + "\"/></td>"
	    + "<td valign='middle' width='100%'>" + html + "</td>"
	    + "</tr></table>";
    }

    return html;
}
