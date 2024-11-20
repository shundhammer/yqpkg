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



#include <QAction>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QKeyEvent>

#include "YQi18n.h"
#include "YQPackageSelectorBase.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgTextDialog.h"
#include "YQPkgObjList.h"
#include "QY2LayoutUtils.h"
#include "QY2CursorHelper.h"

#include "Logger.h"
#include "Exception.h"


using std::max;
using std::string;



YQPackageSelectorBase::YQPackageSelectorBase( QWidget * parent,
					      long	modeFlags )
    : QFrame( parent )
    , _modeFlags( modeFlags )
{
    _showChangesDialog		= false;
    _pkgConflictDialog		= 0;
    _diskUsageList		= 0;
    _pkgConflictDialog 		= 0;

    // YQUI::setTextdomain( "qt-pkg" );
    // setFont( YQUI::yqApp()->currentFont() );

    _pkgConflictDialog = new YQPkgConflictDialog( this );
    Q_CHECK_PTR( _pkgConflictDialog );

    QString label = _( "Reset &Ignored Dependency Conflicts" );
    _actionResetIgnoredDependencyProblems = new QAction( label, this);
    Q_CHECK_PTR( _actionResetIgnoredDependencyProblems );

    _actionResetIgnoredDependencyProblems->setShortcut((QKeySequence) 0);

    connect( _actionResetIgnoredDependencyProblems, &QAction::triggered,
             this,                                  &YQPackageSelectorBase::resetIgnoredDependencyProblems );

    zyppPool().saveState<zypp::Package  >();
    zyppPool().saveState<zypp::Pattern  >();
    zyppPool().saveState<zypp::Patch    >();

    logInfo() << "PackageSelectorBase init done" << endl;
}


YQPackageSelectorBase::~YQPackageSelectorBase()
{
    logInfo() << "Destroying PackageSelector" << endl;
}


int
YQPackageSelectorBase::resolveDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	logError() << "No package conflict dialog existing" << endl;
	return QDialog::Accepted;
    }


    busyCursor();
    emit resolvingStarted();

    int result = _pkgConflictDialog->solveAndShowConflicts();

    emit resolvingFinished();
    normalCursor();

    return result;
}


int
YQPackageSelectorBase::verifySystem()
{
    if ( ! _pkgConflictDialog )
    {
	logError() << "No package conflict dialog existing" << endl;
	return QDialog::Accepted;
    }


    busyCursor();
    int result = _pkgConflictDialog->verifySystem();
    normalCursor();

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



void
YQPackageSelectorBase::reject()
{
    bool changes =
	zyppPool().diffState<zypp::Package  >()	||
	zyppPool().diffState<zypp::Pattern  >()	||
	zyppPool().diffState<zypp::Patch    >();

    if ( changes )
    {
	if ( zyppPool().diffState<zypp::Package>() )
	    logInfo() << "diffState() reports changed packages" << endl;

	if ( zyppPool().diffState<zypp::Pattern>() )
	    logInfo() << "diffState() reports changed patterns" << endl;

	if ( zyppPool().diffState<zypp::Patch>() )
	    logInfo() << "diffState() reports changed patches" << endl;
    }

    bool confirm = false;

    if ( changes )
    {
	int result =
	    QMessageBox::warning( this, "",
				  _( "Really abandon all changes and exit?" ),
				  _( "&Yes" ), _( "&No" ), "",
				  1,   // defaultButtonNumber (from 0)
				  1 ); // escapeButtonNumber

	confirm = ( result == 0 );
    }

    if ( ! changes || confirm )
    {
        // Really reject

	zyppPool().restoreState<zypp::Package  >();
	zyppPool().restoreState<zypp::Pattern  >();
	zyppPool().restoreState<zypp::Patch    >();

	logInfo() << "Quitting the application" << endl;

        qApp->quit();
    }
    else
    {
	logInfo() << "Returning to package selector" << endl;

	// User changed his mind - don't reject
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
        logInfo() << "Confirm unsupported packages enabled." << endl;
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

    logInfo() << "Closing PackageSelector with \"Accept\"" << endl;
}


bool
YQPackageSelectorBase::showPendingLicenseAgreements()
{
    logInfo() << "Showing all pending license agreements" << endl;

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
			logInfo() << "Resolvable " << sel->name() << " has a license agreement" << endl;

			if( ! sel->hasLicenceConfirmed() )
			{
			    logDebug() << "Showing license agreement for resolvable " << sel->name() << endl;
			    allConfirmed = YQPkgObjListItem::showLicenseAgreement( sel ) && allConfirmed;
			}
			else
			{
			    logInfo() << "Resolvable " << sel->name()
					   << "'s  license is already confirmed" << endl;
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
YQPackageSelectorBase::closeEvent( QCloseEvent * event )
{
    Q_UNUSED( event );

    logInfo() << "Caught WM_CLOSE" << endl;

    event->ignore();
    reject();
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

