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

#include "YQPkgChangeLogView.h"
#include "YQPkgDescriptionDialog.h"


// how many change log entries should be displayed at most,
// displaying huge changes takes too much time (bsc#1044777)
static const int MAX_DISPLAYED_CHANGES = 512;

YQPkgChangeLogView::YQPkgChangeLogView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgChangeLogView::~YQPkgChangeLogView()
{
    // NOP
}


void
YQPkgChangeLogView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    logDebug() << "Generating changelog..." << endl;

    QString html = htmlStart();
    html += htmlHeading( selectable, false );

    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( installed )
    {
        html += changeLogTable( installed->changelog() );

        int not_displayed = installed->changelog().size() - MAX_DISPLAYED_CHANGES;
        if (not_displayed > 0)
        {
            logWarning() << "Changelog size limit reached, ignoring last "
                << not_displayed << " items" << endl;
            html.append("<p class='note'>"
                + notDisplayedChanges(not_displayed, installed->name() + "-" + installed->edition().asString())
                + "</p>");
        }
    }
    else
    {
	html += "<p><i>" + _( "Information only available for installed packages." ) + "</i></p>";
    }
    html += htmlEnd();

    logDebug() << "Changelog HTML size: " << html.size() << endl;
    setHtml( html );
    logDebug() << "Changes displayed" << endl;
}



QString YQPkgChangeLogView::changeLogTable( const zypp::Changelog & changeLog ) const
{
    logDebug() << "Changelog size: " << changeLog.size() << " entries" << endl;
    QString html;

    int index = 0;
    for ( zypp::Changelog::const_iterator it = changeLog.begin();
	  it != changeLog.end();
	  ++it )
    {
	QString changes = htmlEscape( fromUTF8( (*it).text() ) );
	changes.replace( "\n", "<br>" );
	changes.replace( " ", "&nbsp;" );

	html += row(
		    cell( (*it).date()   ) +
		    cell( (*it).author() ) +
		    "<td valign='top'>" + changes + "</td>" // cell() calls htmlEscape() !
		    );

    if (++index == MAX_DISPLAYED_CHANGES)
        break;
    }

    return html.isEmpty() ? "" : table( html );
}

QString YQPkgChangeLogView::notDisplayedChanges(int missing, const std::string &pkg)
{
    // TRANSLATORS: The package change log is too long to display, only the latest
    // changes are displayed. %1 is the number of the items which are not displayed,
    // %2 contains a command for getting the full changes manually.
    QString msg = _("(%1 more change entries are not displayed. Run \""
        "%2\" to see the complete change log.)");

    QString cmd = QString("rpm -q --changelog %1").arg(pkg.c_str());
    return msg.arg(QString::number(missing), cmd);
}

