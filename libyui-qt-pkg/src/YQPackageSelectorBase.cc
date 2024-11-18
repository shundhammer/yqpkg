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
  File:	      YQPackageSelectorBase.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include "Logger.h"

#include <yui/YEvent.h>
#include <yui/qt/YQUI.h>
#include <yui/qt/YQApplication.h>
#include <yui/qt/YQDialog.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/QY2Styler.h>
#include <yui/qt/utf8.h>

#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QKeyEvent>

#include "YQPackageSelectorBase.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgTextDialog.h"
#include "YQPkgObjList.h"
#include "QY2LayoutUtils.h"


using std::max;
using std::string;



YQPackageSelectorBase::YQPackageSelectorBase( YWidget * parent,
					      long	modeFlags )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YPackageSelector( parent, modeFlags )
{
    setWidgetRep( this );

    _wmCloseHandler		= 0;
    _showChangesDialog		= false;
    _pkgConflictDialog		= 0;
    _diskUsageList		= 0;
    _pkgConflictDialog 		= 0;

    YQUI::setTextdomain( "qt-pkg" );
    setFont( YQUI::yqApp()->currentFont() );

    _pkgConflictDialog = new YQPkgConflictDialog( this );
    Q_CHECK_PTR( _pkgConflictDialog );

    QString label = _( "Reset &Ignored Dependency Conflicts" );
    _actionResetIgnoredDependencyProblems = new QAction( label, this);
    _actionResetIgnoredDependencyProblems->setShortcut((QKeySequence) 0);
    //_actionResetIgnoredDependencyProblems->setMenuRole(QAction::TextHeuristicRole);
    Q_CHECK_PTR( _actionResetIgnoredDependencyProblems );

    connect( _actionResetIgnoredDependencyProblems, &QAction::triggered,
             this,                                  &YQPackageSelectorBase::resetIgnoredDependencyProblems );

    zyppPool().saveState<zypp::Package  >();
    zyppPool().saveState<zypp::Pattern  >();
    zyppPool().saveState<zypp::Patch    >();

    _wmCloseHandler = new YQPkgSelWmCloseHandler( this );

    QY2Styler::styler()->registerWidget( this );

    yuiMilestone() << "PackageSelectorBase init done" << std::endl;
}


YQPackageSelectorBase::~YQPackageSelectorBase()
{
    yuiMilestone() << "Destroying PackageSelector" << std::endl;

    QY2Styler::styler()->unregisterWidget( this );

    if ( _wmCloseHandler )
	delete _wmCloseHandler;
}


int
YQPackageSelectorBase::resolveDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	yuiError() << "No package conflict dialog existing" << std::endl;
	return QDialog::Accepted;
    }


    YQUI::ui()->busyCursor();
    emit resolvingStarted();

    int result = _pkgConflictDialog->solveAndShowConflicts();

    emit resolvingFinished();
    YQUI::ui()->normalCursor();

    return result;
}


int
YQPackageSelectorBase::verifySystem()
{
    if ( ! _pkgConflictDialog )
    {
	yuiError() << "No package conflict dialog existing" << std::endl;
	return QDialog::Accepted;
    }


    YQUI::ui()->busyCursor();
    int result = _pkgConflictDialog->verifySystem();
    YQUI::ui()->normalCursor();

    if ( result == QDialog::Accepted )
    {
	QMessageBox::information( this, "",
				  _( "System dependencies verify OK." ),
				  QMessageBox::Ok );
    }

    return result;
}


int
YQPackageSelectorBase::checkDiskUsage()
{
    if ( ! _diskUsageList )
    {
	return QDialog::Accepted;
    }

    if ( ! _diskUsageList->overflowWarning.inRange() )
	return QDialog::Accepted;

    QString msg =
	// Translators: RichText ( HTML-like ) format
	"<p><b>" + _( "Error: Out of disk space!" ) + "</b></p>"
	+ _( "<p>"
	     "You can choose to install anyway if you know what you are doing, "
	     "but you risk getting a corrupted system that requires manual repairs. "
	     "If you are not absolutely sure how to handle such a case, "
	     "press <b>Cancel</b> now and deselect some packages."
	     "</p>" );

    return YQPkgDiskUsageWarningDialog::diskUsageWarning( msg,
							  100, _( "C&ontinue Anyway" ), _( "&Cancel" ) );
}



void
YQPackageSelectorBase::showAutoPkgList()
{
    resolveDependencies();

    // Show which packages are installed/deleted
    QString msg =
	// Detailed explanation ( automatic word wrap! )
	+ "<p>"
	+ _( "The following items will be changed:"
	     " " )
	+ "<p>";

    YQPkgChangesDialog::showChangesDialog( this,
					   msg,
					   _( "&OK" ),
					   QString(),	// rejectButtonLabel
                                           YQPkgChangesDialog::FilterAutomatic,
					   YQPkgChangesDialog::OptionNone );	// showIfEmpty
}



bool
YQPackageSelectorBase::reject()
{
    bool changes =
	zyppPool().diffState<zypp::Package  >()	||
	zyppPool().diffState<zypp::Pattern  >()	||
	zyppPool().diffState<zypp::Patch    >();

    if ( changes )
    {
	if ( zyppPool().diffState<zypp::Package>() )
	    yuiMilestone() << "diffState() reports changed packages" << std::endl;

	if ( zyppPool().diffState<zypp::Pattern>() )
	    yuiMilestone() << "diffState() reports changed patterns" << std::endl;

	if ( zyppPool().diffState<zypp::Patch>() )
	    yuiMilestone() << "diffState() reports changed patches" << std::endl;
    }

    bool confirm = false;

    if ( changes )
    {
	int result =
	    QMessageBox::warning( this, "",
				  _( "Do you want to Abandon all changes and exit?" ),
				  _( "&Yes" ), _( "&No" ), "",
				  1, // defaultButtonNumber (from 0)
				  1 ); // escapeButtonNumber

	confirm = ( result == 0 );
    }

    if ( ! changes || confirm )
    {
	zyppPool().restoreState<zypp::Package  >();
	zyppPool().restoreState<zypp::Pattern  >();
	zyppPool().restoreState<zypp::Patch    >();

	yuiMilestone() << "Closing PackageSelector with \"Cancel\"" << std::endl;
	YQUI::ui()->sendEvent( new YCancelEvent() );

	return true; 	// Really reject
    }
    else
    {
	yuiMilestone() << "Returning to package selector" << std::endl;

	return false;	// User changed his mind - don't reject
    }
}


void
YQPackageSelectorBase::accept()
{
    bool confirmedAllLicenses;

    do
    {
	// Force final dependency resolving
	if ( resolveDependencies() == QDialog::Rejected )
	    return;

	confirmedAllLicenses = showPendingLicenseAgreements();

    } while ( ! confirmedAllLicenses ); // Some packages will be set to S_TABOO - need another solver run

    if ( _showChangesDialog )
    {
	// Show which packages are installed/deleted automatically
	QString msg =
	    "<p><b>"
	    // Dialog header
	    + _( "Automatic Changes" )
	    + "</b></p>"
	    // Detailed explanation ( automatic word wrap! )
	    + "<p>"
	    + _( "In addition to your manual selections, the following packages"
		 " have been changed to resolve dependencies:" )
	    + "<p>";

	if ( YQPkgChangesDialog::showChangesDialog( this, msg, _( "C&ontinue" ), _( "&Cancel" ), YQPkgChangesDialog::FilterAutomatic, YQPkgChangesDialog::OptionAutoAcceptIfEmpty )
	     == QDialog::Rejected )
	    return;
    }

    if ( confirmUnsupported() )
    {
        yuiMilestone() << "Confirm unsupported packages enabled." << std::endl;
	// Show which packages are unsupported
	
	QString msg =
	    "<p><b>"
	    // Dialog header
	    + _( "Unsupported Packages" )
	    + "</b></p>"
	    // Detailed explanation ( automatic word wrap! )
	    + "<p>"
	    + _( "Please realize that the following selected software is either unsupported or"
		 " requires an additional customer contract for support." )
	    + "<p>";

	if ( YQPkgUnsupportedPackagesDialog::showUnsupportedPackagesDialog( this, msg, _( "C&ontinue" ), _( "&Cancel" ), YQPkgChangesDialog::FilterUser, YQPkgChangesDialog::OptionAutoAcceptIfEmpty )
	     == QDialog::Rejected )
	    return;
    }


    // Check disk usage
    if ( checkDiskUsage() == QDialog::Rejected )
	return;

    yuiMilestone() << "Closing PackageSelector with \"Accept\"" << std::endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "accept" ) );
}

void
YQPackageSelectorBase::repoManager()
{
    yuiMilestone() << "Closing PackageSelector with \"RepoManager\"" << std::endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "repo_mgr" ) );
}

void
YQPackageSelectorBase::onlineUpdateConfiguration()
{
    yuiMilestone() << "Closing PackageSelector with \"OnlineUpdateConfiguration\"" << std::endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "online_update_configuration" ) );
}

void
YQPackageSelectorBase::onlineSearch()
{
    yuiMilestone() << "Closing PackageSelector with \"OnlineSearch\"" << std::endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "online_search" ) );
}

bool
YQPackageSelectorBase::showPendingLicenseAgreements()
{
    yuiMilestone() << "Showing all pending license agreements" << std::endl;

    bool allConfirmed = true;

    if ( onlineUpdateMode() )
	allConfirmed = showPendingLicenseAgreements( zyppPatchesBegin(), zyppPatchesEnd() );

    allConfirmed = showPendingLicenseAgreements( zyppPkgBegin(), zyppPkgEnd() ) && allConfirmed;

    return allConfirmed;
}


bool
YQPackageSelectorBase::showPendingLicenseAgreements( ZyppPoolIterator begin, ZyppPoolIterator end )
{
    bool allConfirmed = true;

    for ( ZyppPoolIterator it = begin; it != end; ++it )
    {
	ZyppSel sel = (*it);

	switch ( sel->status() )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_Update:
	    case S_AutoUpdate:

		if ( sel->candidateObj() )
		{
		    string licenseText = sel->candidateObj()->licenseToConfirm();

		    if ( ! licenseText.empty() )
		    {
			yuiMilestone() << "Resolvable " << sel->name() << " has a license agreement" << std::endl;

			if( ! sel->hasLicenceConfirmed() )
			{
			    yuiDebug() << "Showing license agreement for resolvable " << sel->name() << std::endl;
			    allConfirmed = YQPkgObjListItem::showLicenseAgreement( sel ) && allConfirmed;
			}
			else
			{
			    yuiMilestone() << "Resolvable " << sel->name()
					   << "'s  license is already confirmed" << std::endl;
			}
		    }
		}
		break;

	    default:
		break;
	}
    }

    return allConfirmed;
}


void
YQPackageSelectorBase::notImplemented()
{
    QMessageBox::information( this, "",
			      _( "Not implemented yet. Sorry." ),
			      QMessageBox::Ok );
}


void
YQPackageSelectorBase::resetIgnoredDependencyProblems()
{
    YQPkgConflictDialog::resetIgnoredDependencyProblems();
}


void
YQPackageSelectorBase::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	Qt::KeyboardModifiers special_combo = ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier );

	if ( ( event->modifiers() & special_combo ) == special_combo )
	{
	    if ( event->key() == Qt::Key_A )
	    {
		showAutoPkgList();
		event->accept();
		return;
	    }
	}
    }

    QWidget::keyPressEvent( event );
}


int YQPackageSelectorBase::preferredWidth()
{
    return max( 640, sizeHint().width() );
}


int YQPackageSelectorBase::preferredHeight()
{
    return max( 480, sizeHint().height() );
}


void
YQPackageSelectorBase::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void
YQPackageSelectorBase::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


bool
YQPackageSelectorBase::setKeyboardFocus()
{
    setFocus();

    return true;
}


YEvent *
YQPkgSelWmCloseHandler::filter( YEvent * event )
{
    if ( event && event->eventType() == YEvent::CancelEvent	// WM_CLOSE
	 && ! _inReject )		// prevent recursion
    {
	// Handle WM_CLOSE like "Cancel"
	yuiMilestone() << "Caught WM_CLOSE from package selector dialog" << std::endl;

	YUI::app()->normalCursor();
	YUI_CHECK_WIDGET( _pkgSel );
	
	_inReject = true;	// reject() might send a CancelEvent, too
	bool reallyReject = _pkgSel->reject();
	_inReject = false;

	if ( ! reallyReject )
	{
	    event = 0;		// Stop processing this event
	    yuiMilestone() << "User changed his mind - discarding CancelEvent" << std::endl;
	}
    }

    return event;		// Don't stop processing this event
}


