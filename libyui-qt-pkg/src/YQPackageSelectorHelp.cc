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
  File:	      YQPackageSelectorHelp.cc
  See also:   YQPackageSelector.cc

  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include "Logger.h"

#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <qstring.h>
#include <qbuffer.h>
#include <qapplication.h>

#include "YQPackageSelector.h"
#include "YQPkgTextDialog.h"



void
YQPackageSelector::help()
{
    // Translators: Headline for help about the package manager in general

    QString html = YQPkgTextDialog::htmlHeading( _( "The YaST Package Manager" ) );

    // Translators: Automatic word-wrapping.
    html += para( _( "<b>Note:</b> This is a just a short  overview."
		     " Refer to the manual for details." ) );

    if ( onlineUpdateMode() )
    {
	// Help specific to online update mode

	html += para( _( "In this dialog, select patches to download and install." ) );
	html += para( _( "The list on the left side contains available patches"
			 " along with the respective patch kind (security, recommended, or optional)"
			 " and the (estimated) download size." ) );
	html += para( _( "This list normally contains only those patches that are not installed on your system yet."
			 " You can change that with the <b>Include Installed Patches</b> check box below the list." ) );
	html += para( _( "The <b>Patch Description</b> field contains a longer explanation of the currently"
			 " selected patch. Click a patch in the list to view its description here." ) );
	html += para( _( "The package list on the right side shows the contents of the currently selected patch, i.e.,"
			 " the packages it contains. You cannot install or delete individual packages from a patch,"
			 " only the patch as a whole. This is intentional to avoid system inconsistencies." ) );

	// Translators: Please keep the reference to "filter views" to distinguish between "filter views" that
	// affect the amount of visible packages in the package list and "details views" ( below the package list )
	// that show details about the ( one ) currently selected package in the package list.

	html += para( _( "In addition to <b>Patches</b>, you can also select one of the other filter views"
			 " from <b>Filter</b> at the upper left:" ) );
	html += "<ul>";
    }
    else // ! _onlineUpdateMode
    {
	// Help specific to normal (non-online-update) mode

	html += para( _( "In this dialog, select which packages to install, update, or delete."
			 " You can select individual packages or entire package \"selections\"." ) );
	html += para( _( "Click the status icon for a package or selection to change the status"
			 " or right-click it to open a context menu." ) );
	html += para( _( "Use the <b>Check Dependencies</b> button to resolve package dependencies."
			 " Some packages require other packages to be installed."
			 " Some packages can only be installed if certain other packages are not installed, too."
			 " This check will automatically mark required packages for installation"
			 " and it will warn you if there are dependency conflicts." ) );
	html += para( _( "When you leave this dialog with <b>Accept</b>, this check will automatically be performed." ) );

	// Translators: Please keep the reference to "filter views" to distinguish between "filter views" that
	// affect the amount of visible packages in the package list and "details views" (below the package list)
	// that show details about the (one) currently selected package in the package list.

	html += para( _( "Select one of the available filter views with the <b>Filter</b> combo-box"
			 " at the upper left:" ) );
	html += "<ul>";

	html += listItem( para( _( "<b>Selections</b> shows some predefined sets of packages that logically belong together." ) ) +
			  para( _( "Use the check box next to the selection to select it as a whole."
				   " You can also select or deselect individual packages in the package list at the right." ) ) );

    }

    // Help common to all modes: Description of the various filter views


    html += listItem( para( _( "<b>Package Groups</b> shows packages by category. You can expand and collapse tree items"
			       " to refine or generalize categories. Click any category to display the packages"
			       " in that category in the package list on the right side." ) ) +
		      para( _( " <b>Hint:</b> There is a \"zzz All\" entry at the very end of the list that will show"
			       " all packages. This may take a few seconds on slow machines." ) ) );

    html += listItem( para( _( "<b>Search</b> allows you to search for packages that meet various criteria."
			       " This is usually the easiest way to find a package if you know its name." ) )+
		      para( _( "<b>Hint:</b> You can also use this to find out what package contains a certain library. "
			       " Search in the <b>Provides</b> RPM field." ) ) );

    html += listItem( para( _( "<b>Installation Summary</b> by default shows the changes to your system -- what packages"
			       " will be installed, deleted, or updated." ) ) +
		      para( _( "It is generally a good idea to use <b>Check Dependencies</b> then switch to"
			       " <b>Installation Summary</b> before clicking <b>Accept</b>. This way you can see"
			       " all changes that will be made to your system." ) ) +
		      para( _( "You can also explicitly select what packages with what status to see here;"
			       " use the check boxes at the left side." ) ) +
		      para( _( "<b>Hint:</b> You can also reverse the effect of this filter."
			       " You can see what packages remain the same on your system. Simply check <b>Keep</b>"
			       " and uncheck everything else." ) ) );

    html += "</ul>";

    YQPkgTextDialog::showText( this, html );
}


void
YQPackageSelector::symbolHelp()
{
    // Make sure all images used here are specified in
    // helpimages_DATA in include/Makefile.am !

    // Translators: Headline for help about package status icons
    QString html = YQPkgTextDialog::htmlHeading( _( "Symbols Overview" ) );

    html += "<br>";
    html += "<table border='1'>";

    html += symHelp( "package-available",
		     // Translators: Package status short (!) description
		     _( "Do not install" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is not installed and it will not be installed." ) );

    html += symHelp( "package-install",
		     // Translators: Package status short (!) description
		     _( "Install" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package will be installed. It is not installed yet." ) );

    html += symHelp( "package-installed-updated",
		     // Translators: Package status short (!) description
		     _( "Keep" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Leave it untouched." ) );

    html += symHelp( "package-upgrade",
		     // Translators: Package status short (!) description
		     _( "Update" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Update it or reinstall it"
			" (if the versions are the same)." ) );

    html += symHelp( "package-remove",
		     // Translators: Package status short (!) description
		     _( "Delete" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Delete it." ) );

    html += symHelp( "package-available-locked",
		     // Translators: Package status short (!) description
		     _( "Taboo" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is not installed and should not be installed under any circumstances, "
			" especially not because of unresolved dependencies that other packages"
			" might have or get." )
		     + " "
		     + _( "Packages set to \"taboo\" are treated as if they did not exist on any installation media." ) );

    html += symHelp( "package-installed-locked",
		     // Translators: Package status short (!) description
		     _( "Protected" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is installed and should not be modified, "
			" especially not because of unresolved dependencies that other packages"
			" might have or get." )
		     + " "
		     + _( "Use this status for third-party packages that should not be overwritten by newer versions"
			  " that may come with the distribution." ) );

    html += symHelp( "package-install-auto",
		     // Translators: Package status short (!) description
		     _( "Autoinstall" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package will be installed automatically because some other package needs it." )
		     + " "
		     + _( "<b>Hint:</b> You may have to use \"taboo\" to get rid of such a package." ) );

    html += symHelp( "package-upgrade-auto",
		     // Translators: Package status short (!) description
		     _( "Autoupdate" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed, but some other package"
			" needs a newer version, so it will automatically be updated." ) );

    html += symHelp( "package-remove-auto",
		     // Translators: Package status short (!) description
		     _( "Autodelete" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed, but package dependencies require that it is deleted." )
		     + _( "This can happen, for example, if some other package obsoletes this one." ) );

    html += "</table>";

    YQPkgTextDialog::showText( this, html );
}


QString
YQPackageSelector::symHelp( const QString & imgFileName,
			    const QString & summary,
			    const QString & explanation		)
{
    QIcon icon = YQUI::ui()->loadIcon( imgFileName.toStdString() );
    QPixmap pixmap = icon.pixmap(16);
    QString html = "<tr valign='top'>";
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    html += QString("<td><img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/></td>";
    html += "<td>" + summary 	+ "</td>"
	+ "<td>" + explanation 	+ "</td>"
	+ "</tr>";

    return html;
}


void
YQPackageSelector::keyboardHelp()
{
    // Translators: Headline for help about "magic keys" in the package manager
    QString html = YQPkgTextDialog::htmlHeading( _( "Special Keys Overview" ) );

    // Translators: Additional hint that will be added to any of the following keyboard actions.
    // Automatic word wrapping.
    QString goto_next = "<br>" + _( "In any case, go to the next list item." );

    html += "<br>";
    html += "<table border=1>";

    html += keyHelp( "+",
		     // Translators: Keyboard action short (!) description
		     _( "Add" ),
		     // Translators: Automatic word-wrapping.
		     _( "Get this package. Install it if it is not installed yet."
			" Update it to the latest version if it is installed "
			" and there is a newer version." )
		     + goto_next );

    html += keyHelp( "-",
		     // Translators: Keyboard action short (!) description
		     _( "Remove" ),
		     // Translators: Automatic word-wrapping.
		     _( "Get rid of this package. Mark it as \"do not install\" "
			"if it is not installed yet. Delete it if it is installed." )
		     + goto_next );

    html += keyHelp( "&gt;",
		     // Translators: Keyboard action short (!) description
		     _( "Update" ),
		     // Translators: Automatic word-wrapping.
		     _( "Update this package if it is installed and there is a newer version."
			" Ignore packages that are not installed." )
		     + goto_next );


    html += keyHelp( "&lt;",
		     // Translators: Keyboard action short (!) description
		     _( "Undo Update" ),
		     // Translators: Automatic word-wrapping.
		     _( "Undo the effect of \">\" above: Set package to \"keep\""
			" if it is currently set to \"update\". Ignore all other packages." )
		     + goto_next );

    html += keyHelp( "!",
		     // Translators: Keyboard action short (!) description
		     _( "Taboo" ),
		     // Translators: Automatic word-wrapping.
		     _( "Set this package to \"taboo\" if it is not installed:"
			" make sure this package does not get installed, especially not"
			" because of unresolved dependencies that other packages might have or get. " )
		     + " "
		     + _( "Packages set to \"taboo\" are treated as if they did not exist on any installation media." )
		     + "<br>"
		     + goto_next );

    html += keyHelp( "*",
		     // Translators: Keyboard action short (!) description
		     _( "Protect" ),
		     // Translators: Automatic word-wrapping.
		     _( "Set this package to \"protected\" if it is installed:"
			" make sure this package will not be modified, especially not"
			" because of unresolved dependencies that other packages might have or get. " )
		     + " "
		     + _( "Use this for third-party packages that should not be overwritten by newer versions"
			  " that may come with the distribution." )
		     + "<br>"
		     + goto_next );

    html += "</table>";

    YQPkgTextDialog::showText( this, html );
}


QString
YQPackageSelector::keyHelp( const QString & key,
			    const QString & summary,
			    const QString & explanation		)
{
    QString html = "<tr valign='top'>";
    html +=
	  "<td><table><tr><td align='center'><b>"
	+ key
	+ "</b></td></tr></table></td>"
	+ "<td>" + summary 	+ "</td>"
	+ "<td>" + explanation 	+ "</td>"
	+ "</tr>";

    return html;
}


QString
YQPackageSelector::para( const QString & text )
{
    return "<p>" + text + "</p>";
}


QString
YQPackageSelector::listItem( const QString & text )
{
    return "<li>" + text + "</li>";
}


