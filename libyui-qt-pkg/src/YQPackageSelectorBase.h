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
  File:	      YQPackageSelectorBase.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPackageSelectorBase_h
#define YQPackageSelectorBase_h

#include <QEvent>
#include <QFrame>

#include <yui/YEventFilter.h>
#include <yui/YPackageSelector.h>

#include "YQZypp.h"


class QY2ComboTabWidget;
class QAction;

class YQPkgConflictDialog;
class YQPkgDiskUsageList;
class YQPkgSelWmCloseHandler;


/**
 * Abstract base class for package selectors.
 **/
class YQPackageSelectorBase : public QFrame, public YPackageSelector
{
    Q_OBJECT

protected:

    /**
     * Constructor
     *
     * Will initialize package and selection managers and create conflict
     * dialogs.
     **/
    YQPackageSelectorBase( YWidget * parent, long modeFlags = 0 );

    /**
     * Destructor
     **/
    virtual ~YQPackageSelectorBase();


public:
    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabling( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();


public slots:

    /**
     * Resolve dependencies (unconditionally) for all resolvables.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int resolveDependencies();

    /**
     * Verifies dependencies of the currently installed system.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int verifySystem();

    /**
     * Check for disk overflow and post a warning dialog if necessary.
     * The user can choose to override this warning.
     *
     * Returns QDialog::Accepted if no warning is necessary or if the user
     * wishes to override the warning, QDialog::Rejected otherwise.
     **/
    int checkDiskUsage();

    /**
     * Display a list of automatically selected packages
     * (excluding packages contained in any selections that are to be installed)
     **/
    void showAutoPkgList();

    /**
     * Close processing and abandon changes.
     * If there were changes, this will post an "Abandon all changes?" pop-up.
     *
     * Return 'true' if the user really wants to reject (or if there were no
     * changes anyway), 'false' if not.
     **/
    bool reject();

    /**
     * Close processing and accept changes
     **/
    void accept();

    /**
     * Close processing and request start of the repository manager
     **/
    void repoManager();

    /**
     * Close processing and request start of the online update configuration
     **/
    void onlineUpdateConfiguration();

    /**
     * Close processing and request start of the online search
     **/
    void onlineSearch();

    /**
     * Inform user about a feature that is not implemented yet.
     * This should NEVER show up in the final version.
     **/
    void notImplemented();


signals:

    /**
     * Emitted when package resolving is started.
     * This can be used for some kind of "busy display".
     **/
    void resolvingStarted();

    /**
     * Emitted when package resolving is finished.
     **/
    void resolvingFinished();


protected slots:

    /**
     * Reset all previously ignored dependency problems.
     **/
    void resetIgnoredDependencyProblems();


protected:

    /**
     * Show all license agreements the user has not confirmed yet
     * (for all packages that will be installed, and in YOU mode also for
     * patches).
     *
     * Returns 'true' if all licenses were confirmed, 'false' if one or more
     * licenses were not confirmed (in which case some packages might be set to
     * S_TABOO, which might require another resolver run).
     **/
    bool showPendingLicenseAgreements();

    /**
     * Show all license agreements in a resolvable range. To be used with
     * zyppPkgBegin() and zyppPkgEnd() or with zyppPatchesBegin() and
     * zyppPatchesEnd().
     **/
    bool showPendingLicenseAgreements( ZyppPoolIterator begin,
				       ZyppPoolIterator end );

    /**
     * Event handler for keyboard input - for debugging and testing.
     *
     * Reimplemented from QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    YQPkgSelWmCloseHandler *	_wmCloseHandler;

    bool			_showChangesDialog;
    YQPkgConflictDialog *	_pkgConflictDialog;
    YQPkgDiskUsageList *	_diskUsageList;
    QAction *			_actionResetIgnoredDependencyProblems;
};



/**
 * Helper class: Event filter for the WM_CLOSE event
 **/
class YQPkgSelWmCloseHandler: public YEventFilter
{
public:
    YQPkgSelWmCloseHandler( YQPackageSelectorBase * pkgSel )
	: YEventFilter()
	, _pkgSel( pkgSel )
	, _inReject( false )
	{}

    virtual ~YQPkgSelWmCloseHandler() {};

    /**
     * The filter method: This is what this class is all about.
     * Check for Cancel events (WM_CLOSE).
     **/
    virtual YEvent * filter( YEvent * event );

    YQPackageSelectorBase * pkgSel() const { return _pkgSel; }

private:

    YQPackageSelectorBase * 	_pkgSel;
    bool			_inReject;
};


#endif // YQPackageSelectorBase_h
