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
  File:	      YQPkgGenericDetailsView.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

*/


#include "Logger.h"

#include "YQi18n.h"
#include <yui/qt/utf8.h>
#include <yui/qt/QY2Styler.h>

#include <QTabWidget>
#include <QRegExp>
#include <QDateTime>

#include "YQPkgGenericDetailsView.h"


using std::string;


YQPkgGenericDetailsView::YQPkgGenericDetailsView( QWidget * parent )
    : QTextBrowser( parent )
{
    _selectable = 0;
    _parentTab  = dynamic_cast<QTabWidget *> (parent);

    if ( _parentTab )
    {
        connect( _parentTab, &QTabWidget::currentChanged,
                 this,       &YQPkgGenericDetailsView::reloadTab );
    }

    QString css;
    css = "table.stats"
        "{text-align: center;"
        "font-family: Verdana, Geneva, Arial, Helvetica, sans-serif ;"
        "font-weight: normal;"
        "font-size: small;"
        "width: 100%;"
        ""
        "border: 1px;"
        "border-collapse: collapse;"
        "border-spacing: 4px;}"
        ""
        "table.stats td"
        "{"
        "padding: 4px;"
        "text-align: left;"
        "border: 1px; solid;"
        "}"
        ""
        "table.stats td.hed"
        "{"
        "padding: 4px;"
        "text-align: left;"
        "border-bottom: 2px; solid;"
        "font-size: small;"
        "font-weight: bold;} "
        "p.note"
        "{color: grey;"
        "font-style: italic;}";

    document()->addResource( QTextDocument::StyleSheetResource, QUrl( "format.css" ), css );
}


YQPkgGenericDetailsView::~YQPkgGenericDetailsView()
{
    // NOP
}


void
YQPkgGenericDetailsView::reloadTab( int newCurrent )
{
    if ( _parentTab && _parentTab->widget(newCurrent) == this )
    {
	showDetailsIfVisible( _selectable );
    }
}


void
YQPkgGenericDetailsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentWidget() == this )  // Is this page the topmost?
	{
	    showDetails( selectable );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( selectable );
    }
}


QSize
YQPkgGenericDetailsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


QString
YQPkgGenericDetailsView::htmlStart()
{
    return "<html><head>"
        "<link rel='stylesheet' type='text/css' href='format.css'>"
        "</head><body>";
}

QString
YQPkgGenericDetailsView::htmlEnd()
{
    return "</body></html>";
}


QString
YQPkgGenericDetailsView::htmlHeading( ZyppSel selectable, bool showVersion )
{
    if ( ! selectable )
	return "";

    ZyppObj zyppObj = selectable->theObj();

    if ( ! zyppObj )
	return "";

    QString summary = fromUTF8( zyppObj->summary() );

    QString html = "<table";

    if ( ! QY2Styler::styler()->usingAlternateStyleSheet() )
        html +=  " class=\"stats\"";

    html += "><tr><td><b>"
	+ fromUTF8( zyppObj->name() )
	+ "</b>";

    if ( showVersion )
	html += QString( "<b>-" ) + zyppObj->edition().asString().c_str() + "</b>";

    if ( ! summary.isEmpty() )
	html += " - " + summary;

    html += "</td></tr></table>";

    return html;
}



QString
YQPkgGenericDetailsView::htmlEscape( const QString & plainText )
{
    QString html = plainText;
    // logDebug() << "Escaping \"" << plainText << "\"" << endl;

    html.replace( QRegExp( "&" ), "&amp;" );
    html.replace( QRegExp( "<" ), "&lt;"  );
    html.replace( QRegExp( ">" ), "&gt;"  );

    return html;
}


QString
YQPkgGenericDetailsView::table( const QString & contents )
{
    QString html = "<table";
    if ( ! QY2Styler::styler()->usingAlternateStyleSheet() )
        html +=  " class=\"stats\"";

    html += ">" + contents + "</table>";

    return html;
}


QString
YQPkgGenericDetailsView::row( const QString & contents )
{
    return "<tr>" + contents + "</tr>";
}


QString
YQPkgGenericDetailsView::cell( QString contents )
{
    contents = htmlEscape( contents );
    return "<td>" + contents + "</td>";
}


QString
YQPkgGenericDetailsView::cell( int contents )
{
    return QString( "<td>%1</td>" ).arg( contents );
}


QString
YQPkgGenericDetailsView::cell( const zypp::Date & date )
{
    return cell( ( (time_t) date == (time_t) 0 ? "" : date.asString() ) );
}


QString
YQPkgGenericDetailsView::cell( const string & contents )
{
    return cell( fromUTF8( contents ) );
}


QString
YQPkgGenericDetailsView::hcell( QString contents )
{
    return "<td>" + contents + "</td>";
}


