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
  File:	      YQPackageSelector.cc
  See also:   YQPackageSelectorHelp.cc

  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/YEvent.h>
#include <yui/qt/YQUI.h>
#include <yui/qt/YQApplication.h>
#include <yui/qt/YQDialog.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/QY2ComboTabWidget.h>
#include <yui/qt/utf8.h>

#include <zypp/SysContent.h>
#include <zypp/base/String.h>
#include <zypp/base/Sysconfig.h>
#include <boost/bind/bind.hpp>

#include <fstream>
#include <algorithm>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QShortcut>
#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QMap>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTimer>
#include <QMenu>
#include <QSettings>

#include "YQZypp.h"
#include "YQPackageSelector.h"
#include "YQPkgChangeLogView.h"
#include "YQPkgChangesDialog.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"
#include "YQPkgDependenciesView.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQPkgFileListView.h"
#include "YQPkgFilterTab.h"
#include "YQPkgHistoryDialog.h"
#include "YQPkgLangList.h"
#include "YQPkgList.h"
#include "YQPkgClassFilterView.h"
#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "YQPkgPatternList.h"
#include "YQPkgProductDialog.h"
#include "YQPkgRepoFilterView.h"
#include "YQPkgRepoList.h"
#include "YQPkgSearchFilterView.h"
#include "YQPkgServiceFilterView.h"
#include "YQPkgStatusFilterView.h"
#include "YQPkgTechnicalDetailsView.h"
#include "YQPkgTextDialog.h"
#include "YQPkgUpdateProblemFilterView.h"
#include "YQPkgVersionsView.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


using std::max;
using std::string;
using std::map;
using std::pair;

#define CHECK_DEPENDENCIES_ON_STARTUP			1
#define DEPENDENCY_FEEDBACK_IF_OK			1
#define AUTO_CHECK_DEPENDENCIES_DEFAULT			true
#define ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE	0
#define GLOBAL_UPDATE_CONFIRMATION_THRESHOLD		20
#define ENABLE_SOURCE_RPMS				0
#define BRAINDEAD_LIB_NAMING_SCHEME			1
#define MARGIN						6	// around the widget
#define SPACING_BELOW_MENU_BAR				4
#define SPLITTER_HALF_SPACING				4

#define DEFAULT_EXPORT_FILE_NAME	"user-packages.xml"
#define FAST_SOLVER			1

#define SETTINGS_DIR			"YaST2"
#define PATH_TO_YAST_SYSCONFIG		"/etc/sysconfig/yast2"
#define OPTION_VERIFY			"PKGMGR_VERIFY_SYSTEM"
#define OPTION_AUTO_CHECK		"PKGMGR_AUTO_CHECK"
#define OPTION_RECOMMENDED		"PKGMGR_RECOMMENDED"

YQPackageSelector::YQPackageSelector( YWidget *		parent,
				      long		modeFlags )
    : YQPackageSelectorBase( parent, modeFlags )
{
    _showChangesDialog		= true;
    _autoDependenciesAction	= 0;
    _detailsViews		= 0;
    _filters			= 0;
    _langList			= 0;
    _pkgClassFilterView		= 0;
    _patchFilterView		= 0;
    _patchList			= 0;
    _patternList		= 0;
    _pkgChangeLogView		= 0;
    _pkgDependenciesView	= 0;
    _pkgDescriptionView		= 0;
    _pkgFileListView		= 0;
    _pkgList			= 0;
    _pkgTechnicalDetailsView	= 0;
    _pkgVersionsView		= 0;
    _repoFilterView		= 0;
    _serviceFilterView		= 0;
    _searchFilterView		= 0;
    _statusFilterView		= 0;
    _updateProblemFilterView	= 0;
    _excludeDevelPkgs		= 0;
    _excludeDebugInfoPkgs	= 0;

    // VERSION is a command-line #define (-DVERSION="1.2.3") added
    // to the compiler command line by cmake from ../../VERSION.cmake
    logInfo() << "This is libyui-qt-pkg " << VERSION << endl;

    if ( onlineUpdateMode() )	logInfo() << "Online update mode" << endl;
    if ( updateMode() )		logInfo() << "Update mode" << endl;

    basicLayout();
    addMenus();		// Only after all widgets are created!
    loadSettings();	// Only after menus are created!
    makeConnections();
    emit loadData();

    _filters->loadSettings();
    bool pagesRestored = _filters->tabCount() > 0;

    if ( _pkgList )
	_pkgList->clear();

    if ( ! pagesRestored )
    {
	logDebug() << "No page configuration saved, using fallbacks" << endl;

	//
	// Add a number of default tabs in the desired order
	//

	if ( _searchFilterView )		_filters->showPage( _searchFilterView );

	if ( ! searchMode() && ! summaryMode()
	     && _patternList )			_filters->showPage( _patternList );

	if ( _statusFilterView )		_filters->showPage( _statusFilterView );
    }


    //
    // Move the desired tab to the foreground
    //

    if ( _patchFilterView && onlineUpdateMode() )
    {
	if ( _patchFilterView && _patchList )
	{
	    _filters->showPage( _patchFilterView );
	    _patchList->filter();
	}
    }
    else if ( _repoFilterView && repoMode() )
    {
	if ( YQPkgRepoList::countEnabledRepositories() > 1 )
	{
	    _filters->showPage( _repoFilterView );
	    _repoFilterView->filter();
	}
    }
    else if ( _updateProblemFilterView )
    {
	_filters->showPage( _updateProblemFilterView );
	_updateProblemFilterView->filter();
    }
    else if ( searchMode() && _searchFilterView )
    {
	if ( _pkgClassFilterView && anyRetractedPkgInstalled() )
	{
	    // Exceptional case: If the system has any retracted package
	    // installed, switch to that filter view and show those packages.
	    // This should happen only very, very rarely.

	    logInfo() << "Found installed retracted packages; switching to that view" << endl;
	    _filters->showPage( _pkgClassFilterView );
	    _pkgClassFilterView->showPkgClass( YQPkgClassRetractedInstalled );

	    // Also show a pop-up warning?
	    //
	    // This could become very annoying really quickly because you'll
	    // get it with every start of the package selection as long as any
	    // retracted package version is installed (which might be a
	    // deliberate conscious decision by the user). It's also not easy
	    // to add a "Don't show this again" checkbox in such a pop-up;
	    // which retracted packages are installed might change between
	    // program runs, and we'd have to inform the user when such a
	    // change occurs.
	}
	else
	{
	    // Normal case: Show the "Search" filter view.

	    _filters->showPage( _searchFilterView );
	    _searchFilterView->filter();
	    QTimer::singleShot( 0, _searchFilterView, SLOT( setFocus() ) );
	}
    }
    else if ( summaryMode() && _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->filter();
	_pkgList->selectNextItem();
    }
    else if ( _patternList )
    {
	_filters->showPage( _patternList );
	_patternList->filter();
    }


    if ( _filters->diskUsageList() )
	_filters->diskUsageList()->updateDiskUsage();

    logInfo() << "PackageSelector init done" << endl;


#if CHECK_DEPENDENCIES_ON_STARTUP

    if ( ! testMode() )
    {
	// Fire up the first dependency check in the main loop.
	// Don't do this right away - wait until all initializations are finished.
	QTimer::singleShot( 0, this, SLOT( resolveDependencies() ) );

    }
#endif

}


void
YQPackageSelector::basicLayout()
{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout( layout );
    layout->setContentsMargins( MARGIN,		// left
				0,		// top
				MARGIN,		// right
				MARGIN );	// bottom
    layout->setSpacing( SPACING_BELOW_MENU_BAR );
    layoutMenuBar( this );

    QString settingsName = "YQPackageSelector";

    if ( onlineUpdateMode() )	settingsName = "YQOnlineUpdate";
    if ( updateMode() )		settingsName = "YQSystemUpdate";

    _filters = new YQPkgFilterTab( this, settingsName );
    CHECK_NEW( _filters );

    layout->addWidget( _filters );
    layoutFilters( this );
    layoutRightPane( _filters->rightPane() );
}


void
YQPackageSelector::layoutFilters( QWidget *parent )
{
    //
    // Update problem view
    //

    if ( updateMode() )
    {
	if ( YQPkgUpdateProblemFilterView::haveProblematicPackages()
	     || testMode() )
	{
	    _updateProblemFilterView = new YQPkgUpdateProblemFilterView( parent );
	    CHECK_NEW( _updateProblemFilterView );
	    _filters->addPage( _( "&Update Problems" ), _updateProblemFilterView, "update_problems" );
	}
    }


    //
    // Patches view
    //

    if ( onlineUpdateMode()
#if ALWAYS_SHOW_PATCHES_VIEW_IF_PATCHES_AVAILABLE
	 || ! zyppPool().empty<zypp::Patch>()
#endif
	 )
    {
	addPatchFilterView();
    }


    //
    // Patterns view
    //

    if ( ! zyppPool().empty<zypp::Pattern>() || testMode() )
    {
	_patternList = new YQPkgPatternList( parent, true );
	CHECK_NEW( _patternList );
	_filters->addPage( _( "Patter&ns" ), _patternList, "patterns" );

	connect( _patternList,		SIGNAL( statusChanged()			),
		 this,			SLOT  ( autoResolveDependencies()	) );

	connect( this,			SIGNAL( refresh()			),
		 _patternList,		SLOT  ( updateItemStates()		) );

	if ( _pkgConflictDialog )
	{
	    connect( _pkgConflictDialog, SIGNAL( updatePackages()		),
		     _patternList,	 SLOT  ( updateItemStates()		) );
	}
    }


    //
    // Package classification view
    //

    _pkgClassFilterView = new YQPkgClassFilterView( parent );
    CHECK_NEW( _pkgClassFilterView );
    _filters->addPage( _( "Package &Classification" ), _pkgClassFilterView, "package_classification" );

    connect( this,			SIGNAL( loadData() ),
	     _pkgClassFilterView,	SLOT  ( filter()   ) );


    //
    // Languages view
    //

    _langList = new YQPkgLangList( parent );
    CHECK_NEW( _langList );

    _filters->addPage( _( "&Languages" ), _langList, "languages" );
    _langList->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored ) ); // hor/vert

    connect( _langList,		SIGNAL( statusChanged()			),
	     this,		SLOT  ( autoResolveDependencies()	) );

    connect( this,		SIGNAL( refresh()			),
	     _langList,		SLOT  ( updateItemStates()		) );


    //
    // Repository view
    //

    _repoFilterView = new YQPkgRepoFilterView( parent );
    CHECK_NEW( _repoFilterView );
    _filters->addPage( _( "&Repositories" ), _repoFilterView, "repos" );
    // hide and show the upgrade label when tabs change, or when the user
    // selects repositories
    connect(_repoFilterView, SIGNAL(filterStart()), this, SLOT(updateRepositoryUpgradeLabel()));
    connect(this, SIGNAL(refresh()), this, SLOT(updateRepositoryUpgradeLabel()));
    connect(_filters, &YQPkgFilterTab::currentChanged,
	    this,     &YQPackageSelector::updateRepositoryUpgradeLabel );

    // Services view - only if a service is present
    if (YQPkgServiceFilterView::any_service())
    {
	_serviceFilterView = new YQPkgServiceFilterView( parent );
	CHECK_NEW( _serviceFilterView );

	// TRANSLATORS: Menu item
	_filters->addPage( _( "&Services" ), _serviceFilterView, "services" );
    }

    //
    // Package search view
    //

    _searchFilterView = new YQPkgSearchFilterView( parent );
    CHECK_NEW( _searchFilterView );
    _filters->addPage( _( "S&earch" ), _searchFilterView, "search" );



#if 0
    // DEBUG

    _filters->addPage( _( "&Keywords"	), new QLabel( "Keywords\nfilter\n\nfor future use", this ), "keywords" );
#endif

    //
    // Status change view
    //

    _statusFilterView = new YQPkgStatusFilterView( parent );
    CHECK_NEW( _statusFilterView );
    _filters->addPage( _( "&Installation Summary" ), _statusFilterView, "inst_summary" );
}


YQPackageSelector::~YQPackageSelector()
{
    saveSettings();
}


QWidget *
YQPackageSelector::layoutRightPane( QWidget *parent )
{
    QVBoxLayout *layout = new QVBoxLayout( parent );
    CHECK_NEW( layout );
    layout->setContentsMargins( SPLITTER_HALF_SPACING,	// left
				0,			// top
				0,			// right
				0 );			// bottom

    QSplitter * splitter = new QSplitter( Qt::Vertical, parent );
    CHECK_NEW( splitter );
    layout->addWidget(splitter);

    layoutPkgList( splitter );
    layoutDetailsViews( splitter );
    layoutButtons( parent );

    return parent;
}


void
YQPackageSelector::layoutPkgList( QWidget *parent )
{
    // this is made visible when activating the repository
    // filter
    QWidget *_notificationsContainer = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(_notificationsContainer);

    _repoUpgradingLabel = new QLabel(_notificationsContainer);
    _repoUpgradingLabel->setTextFormat(Qt::RichText);
    _repoUpgradingLabel->setWordWrap(true);
    _repoUpgradingLabel->setVisible(false);

    _repoUpgradeLabel = new QLabel(_notificationsContainer);
    _repoUpgradeLabel->setTextFormat(Qt::RichText);
    _repoUpgradeLabel->setWordWrap(true);
    _repoUpgradeLabel->setVisible(false);
    _repoUpgradeLabel->setObjectName( "RepoUpgradeLabel");

    layout->addWidget(_repoUpgradingLabel);
    layout->addWidget(_repoUpgradeLabel);

    // if the user clicks on a link on the label, we have to check
    // which repository upgrade job to add or remove, for that
    // we will encode the links as repoupgradeadd://alias and
    // repoupgraderemove:://alias
    connect( _repoUpgradeLabel, SIGNAL( linkActivated		       ( QString ) ),
	     this,		SLOT  ( slotRepoUpgradeLabelLinkClicked( QString ) ) );

    connect(_repoUpgradingLabel, SIGNAL( linkActivated			( QString ) ),
	    this,		 SLOT  ( slotRepoUpgradeLabelLinkClicked( QString ) ) );

    updateRepositoryUpgradeLabel();

    _pkgList= new YQPkgList( parent );
    CHECK_NEW( _pkgList );

    connect( _pkgList,	SIGNAL( statusChanged()		  ),
	     this,	SLOT  ( autoResolveDependencies() ) );
}


void
YQPackageSelector::layoutDetailsViews( QWidget *parent )
{
    bool haveInstalledPkgs = YQPkgList::haveInstalledPkgs();


    _detailsViews = new QTabWidget( parent );
    CHECK_NEW( _detailsViews );

    //
    // Description
    //

    _pkgDescriptionView = new YQPkgDescriptionView( _detailsViews, confirmUnsupported() );
    CHECK_NEW( _pkgDescriptionView );

    _detailsViews->addTab( _pkgDescriptionView, _( "D&escription" ) );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,			SIGNAL( currentItemChanged  ( ZyppSel ) ),
	     _pkgDescriptionView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    //
    // Technical details
    //

    _pkgTechnicalDetailsView = new YQPkgTechnicalDetailsView( _detailsViews );
    CHECK_NEW( _pkgTechnicalDetailsView );

    _detailsViews->addTab( _pkgTechnicalDetailsView, _( "&Technical Data" ) );

    connect( _pkgList,			SIGNAL( currentItemChanged  ( ZyppSel ) ),
	     _pkgTechnicalDetailsView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );


    //
    // Dependencies
    //

    _pkgDependenciesView = new YQPkgDependenciesView( _detailsViews );
    CHECK_NEW( _pkgDependenciesView );

    _detailsViews->addTab( _pkgDependenciesView, _( "Dependencies" ) );
    _detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    connect( _pkgList,			SIGNAL( currentItemChanged  ( ZyppSel ) ),
	     _pkgDependenciesView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );



    //
    // Versions
    //

    _pkgVersionsView = new YQPkgVersionsView( _detailsViews );
    CHECK_NEW( _pkgVersionsView );

    _detailsViews->addTab( _pkgVersionsView, _( "&Versions" ) );

    connect( _pkgList,		SIGNAL( currentItemChanged  ( ZyppSel ) ),
	     _pkgVersionsView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    connect( _pkgList,		SIGNAL( statusChanged() ),
	     _pkgVersionsView,	SIGNAL( statusChanged() ) );


    //
    // File List
    //

    if ( haveInstalledPkgs )	// file list information is only available for installed pkgs
    {
	_pkgFileListView = new YQPkgFileListView( _detailsViews );
	CHECK_NEW( _pkgFileListView );

	_detailsViews->addTab( _pkgFileListView, _( "File List" ) );
	_detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

	connect( _pkgList,		SIGNAL( currentItemChanged  ( ZyppSel ) ),
		 _pkgFileListView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );
    }


    //
    // Change Log
    //

    if ( haveInstalledPkgs )	// change log information is only available for installed pkgs
    {
	_pkgChangeLogView = new YQPkgChangeLogView( _detailsViews );
	CHECK_NEW( _pkgChangeLogView );

	_detailsViews->addTab( _pkgChangeLogView, _( "Change Log" ) );
	_detailsViews->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

	connect( _pkgList,		SIGNAL( currentItemChanged    ( ZyppSel ) ),
		 _pkgChangeLogView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );
    }
}


void
YQPackageSelector::layoutButtons( QWidget *parent )
{
    QWidget * button_box = new QWidget( parent );
    CHECK_NEW( button_box );
    parent->layout()->addWidget( button_box );

    QHBoxLayout * layout = new QHBoxLayout( button_box );
    CHECK_NEW( layout );

    button_box->setLayout( layout );
    layout->setContentsMargins( 2,	// left
				2,	// top
				2,	// right
				2 );	// bottom
    layout->addStretch();

    QPushButton * cancel_button = new QPushButton( _( "&Cancel" ), button_box );
    CHECK_NEW( cancel_button );
    layout->addWidget(cancel_button);

    cancel_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( cancel_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( reject()   ) );


    QPushButton * accept_button = new QPushButton( _( "&Accept" ), button_box );
    CHECK_NEW( accept_button );
    layout->addWidget(accept_button);
    accept_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( accept_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( accept()   ) );

    button_box->setFixedHeight( button_box->sizeHint().height() );
}


void
YQPackageSelector::layoutMenuBar( QWidget *parent )
{
    _menuBar = new QMenuBar( parent );
    CHECK_NEW( _menuBar );
    parent->layout()->addWidget(_menuBar);

    _fileMenu		= 0;
    _optionsMenu	= 0;
    _pkgMenu		= 0;
    _patchMenu		= 0;
    _extrasMenu		= 0;
    _configMenu		= 0;
    _dependencyMenu	= 0;
    _helpMenu		= 0;

}


void
YQPackageSelector::addMenus()
{
    //
    // File menu
    //

    _fileMenu = new QMenu( _menuBar );
    CHECK_NEW( _fileMenu );
    QAction * action = _menuBar->addMenu( _fileMenu );
    action->setText( _( "&File" ));

    _fileMenu->addAction( _( "&Import..." ),	this, SLOT( pkgImport() ) );
    _fileMenu->addAction( _( "&Export..." ),	this, SLOT( pkgExport() ) );

    _fileMenu->addSeparator();

    _fileMenu->addAction( _( "E&xit -- Discard Changes" ), this, SLOT( reject() ) );
    _fileMenu->addAction( _( "&Quit -- Save Changes"	 ), this, SLOT( accept() ) );


    if ( _pkgList )
    {
	//
	// Package menu
	//

	_pkgMenu = new QMenu( _menuBar );
	CHECK_NEW( _pkgMenu );
	action = _menuBar->addMenu( _pkgMenu );
	action->setText(_( "&Package" ));

	_pkgMenu->addAction(_pkgList->actionSetCurrentInstall);
	_pkgMenu->addAction(_pkgList->actionSetCurrentDontInstall);
	_pkgMenu->addAction(_pkgList->actionSetCurrentKeepInstalled);
	_pkgMenu->addAction(_pkgList->actionSetCurrentDelete);
	_pkgMenu->addAction(_pkgList->actionSetCurrentUpdate);
	_pkgMenu->addAction(_pkgList->actionSetCurrentUpdateForce);
	_pkgMenu->addAction(_pkgList->actionSetCurrentTaboo);

#if ENABLE_SOURCE_RPMS
	_pkgMenu->addSeparator();

	_pkgMenu->addAction(_pkgList->actionInstallSourceRpm);
	_pkgMenu->addAction(_pkgList->actionDontInstallSourceRpm);
#endif

	_pkgMenu->addSeparator();
	QMenu * submenu = _pkgList->addAllInListSubMenu( _pkgMenu );
	CHECK_NEW( submenu );

#if ENABLE_SOURCE_RPMS
	submenu->addSeparator();

	_pkgMenu->addAction(_pkgList->actionInstallListSourceRpms);
	_pkgMenu->addAction(_pkgList->actionDontInstallListSourceRpms);
#endif

	//
	// Submenu for all packages
	//

	submenu = new QMenu( _pkgMenu );
	CHECK_NEW( submenu );

	// Translators: Unlike the "all in this list" submenu, this submenu
	// refers to all packages globally, not only to those that are
	// currently visible in the packages list.
	action = _pkgMenu->addMenu( submenu );
	action->setText(_( "All Packages" ));

	submenu->addAction( _( "Update if newer version available" ),
			    this, SLOT( globalUpdatePkg() ) );

	submenu->addAction( _( "Update unconditionally" ),
			    this, SLOT( globalUpdatePkgForce() ) );
    }


    if ( _patchList )
    {
	//
	// Patch menu
	//

	_patchMenu = new QMenu( _menuBar );
	CHECK_NEW( _patchMenu );
	action = _menuBar->addMenu( _patchMenu );
	action->setText(_( "&Patch" ));

	_patchMenu->addAction(_patchList->actionSetCurrentInstall);
	_patchMenu->addAction(_patchList->actionSetCurrentDontInstall);
	_patchMenu->addAction(_patchList->actionSetCurrentKeepInstalled);

#if ENABLE_DELETING_PATCHES
	_patchMenu->addAction(_patchList->actionSetCurrentDelete);
#endif
	_patchMenu->addAction(_patchList->actionSetCurrentUpdate);
	_patchMenu->addAction(_patchList->actionSetCurrentUpdateForce);
	_patchMenu->addAction(_patchList->actionSetCurrentTaboo);

	_patchMenu->addSeparator();
	_patchList->addAllInListSubMenu( _patchMenu );
    }


    //
    // Configuration menu
    //

    if ( repoMgrEnabled() )
    {
	_configMenu = new QMenu( _menuBar );
	CHECK_NEW( _configMenu );
	action = _menuBar->addMenu( _configMenu );
	action->setText(_( "Confi&guration" ));
	_configMenu->addAction( _( "&Repositories..."  ), this, SLOT( repoManager() ), Qt::CTRL + Qt::Key_R );
	_configMenu->addAction( _( "&Online Update..." ), this, SLOT( onlineUpdateConfiguration() ), Qt::CTRL + Qt::Key_O );
    }


    //
    // Dependency menu
    //

    _dependencyMenu = new QMenu( _menuBar );
    CHECK_NEW( _dependencyMenu );
    action = _menuBar->addMenu( _dependencyMenu );
    action->setText(_( "&Dependencies" ));

    _dependencyMenu->addAction( _( "&Check Now" ), this, SLOT( manualResolvePackageDependencies() ) );

    _autoDependenciesAction = new QAction( _( "&Autocheck" ), this );
    _autoDependenciesAction->setCheckable( true );
    _dependencyMenu->addAction( _autoDependenciesAction );

    _installRecommendedAction = _dependencyMenu->addAction(
							   _("Install &Recommended Packages"),
							   this, SLOT (pkgInstallRecommendedChanged(bool)));
    _installRecommendedAction->setCheckable( true );


    //
    // View menu
    //

    _optionsMenu = new QMenu( _menuBar );
    CHECK_NEW( _optionsMenu );
    action = _menuBar->addMenu( _optionsMenu );
    // Translators: Menu for view options (Use a noun, not a verb!)
    action->setText(_( "&Options" ));

    // Translators: This is about packages ending in "-devel", so don't translate that "-devel"!
    _showDevelAction = _optionsMenu->addAction( _( "Show -de&vel Packages" ),
						this, SLOT( pkgExcludeDevelChanged( bool ) ), Qt::Key_F7 );
    _showDevelAction->setCheckable(true);

    _excludeDevelPkgs = new YQPkgObjList::ExcludeRule( _pkgList, QRegExp( ".*(\\d+bit)?-devel(-\\d+bit)?$" ), _pkgList->nameCol() );
    CHECK_NEW( _excludeDevelPkgs );
    _excludeDevelPkgs->enable( false );

    // Translators: This is about packages ending in "-debuginfo", so don't translate that "-debuginfo"!
    _showDebugAction = _optionsMenu->addAction( _( "Show -&debuginfo/-debugsource Packages" ),
						this, SLOT( pkgExcludeDebugChanged( bool ) ), Qt::Key_F8 );
    _showDebugAction->setCheckable(true);
    _excludeDebugInfoPkgs = new YQPkgObjList::ExcludeRule( _pkgList, QRegExp( ".*(-\\d+bit)?-(debuginfo|debugsource)(-32bit)?$" ), _pkgList->nameCol() );
    CHECK_NEW( _excludeDebugInfoPkgs );
    _excludeDebugInfoPkgs->enable( false );


    _verifySystemModeAction = _optionsMenu->addAction( _( "&System Verification Mode" ),
						       this, SLOT( pkgVerifySytemModeChanged( bool ) ) );
    _verifySystemModeAction->setCheckable(true);

    // Widget styles can use the text information in the rendering for sections,
    // or can choose to ignore it and render sections like simple separators.
    _optionsMenu->addSection( _( "Options for this run only..." ) );

    _cleanDepsOnRemoveAction = _optionsMenu->addAction( _( "&Cleanup when deleting packages" ),
							this, SLOT( pkgCleanDepsOnRemoveChanged( bool ) ) );
    _cleanDepsOnRemoveAction->setCheckable(true);

    _allowVendorChangeAction = _optionsMenu->addAction( _( "&Allow vendor change" ),
							this, SLOT( pkgAllowVendorChangeChanged( bool ) ) );
    _allowVendorChangeAction->setCheckable(true);


    //
    // Extras menu
    //

    _extrasMenu = new QMenu( _menuBar );
    CHECK_NEW( _extrasMenu );
    action = _menuBar->addMenu( _extrasMenu );
    action->setText(_( "E&xtras" ));

    _extrasMenu->addAction( _( "Show &Products"		), this, SLOT( showProducts() ) );
    _extrasMenu->addAction( _( "Show P&ackage Changes"	), this, SLOT( showAutoPkgList() ), Qt::CTRL + Qt::Key_A );
    _extrasMenu->addAction( _( "Show &History"		), this, SLOT( showHistory() ) );

    _extrasMenu->addSeparator();

#if BRAINDEAD_LIB_NAMING_SCHEME
    // See bug #434042: libcddb2 vs. libcddb-devel
#else
    // Translators: This is about packages ending in "-devel", so don't translate that "-devel"!
    _extrasMenu->addAction( _( "Install All Matching -&devel Packages" ), this, SLOT( installDevelPkgs() ) );
#endif

    // Translators: This is about packages ending in "-debuginfo", so don't translate that "-debuginfo"!
    _extrasMenu->addAction( _( "Install All Matching -de&buginfo Packages" ), this, SLOT( installDebugInfoPkgs() ) );

    // Translators: This is about packages ending in "-debugsource", so don't translate that "-debugsource"!
    _extrasMenu->addAction( _( "Install All Matching -debug&source Packages" ), this, SLOT( installDebugSourcePkgs() ) );

    _extrasMenu->addAction( _( "Install All Matching &Recommended Packages" ),
			    this, SLOT( installRecommendedPkgs() ) );

    _extrasMenu->addSeparator();

    if ( _pkgConflictDialog )
	_extrasMenu->addAction( _( "Generate Dependency Resolver &Test Case" ),
				_pkgConflictDialog, SLOT( askCreateSolverTestCase() ) );

    if ( _actionResetIgnoredDependencyProblems )
	_extrasMenu->addAction(_actionResetIgnoredDependencyProblems);

#ifdef FIXME
    if ( _patchList )
	_extrasMenu->addAction(_patchList->actionShowRawPatchInfo);
#endif

    if ( (onlineSearchEnabled()) ) {
	_extrasMenu->addAction(_("Search &Online"), this, SLOT( onlineSearch() ) );
    }

    //
    // Help menu
    //

    _helpMenu = new QMenu( _menuBar );
    CHECK_NEW( _helpMenu );
    _menuBar->addSeparator();
    action = _menuBar->addMenu( _helpMenu );
    action->setText(_( "&Help" ));

    // Note: The help functions and their texts are moved out
    // to a separate source file YQPackageSelectorHelp.cc

    // Menu entry for help overview
    _helpMenu->addAction( _( "&Overview" ), this, SLOT( help()		), Qt::Key_F1 );

    // Menu entry for help about used symbols ( icons )
    _helpMenu->addAction( _( "&Symbols" ), this, SLOT( symbolHelp()	), Qt::SHIFT + Qt::Key_F1 );

    // Menu entry for keyboard help
    _helpMenu->addAction( _( "&Keys" ), this, SLOT( keyboardHelp() )	);
}


void
YQPackageSelector::connectFilter( QWidget * filter,
				  QWidget * pkgList,
				  bool hasUpdateSignal )
{
    if ( ! filter  )	return;
    if ( ! pkgList )	return;

    if ( _filters )
    {
	connect( _filters,	SIGNAL( currentChanged(QWidget *) ),
		 filter,	SLOT  ( filterIfVisible()	     ) );
    }

    connect( this,	SIGNAL( refresh()	  ),
	     filter,	SLOT  ( filterIfVisible() ) );

    connect( filter,	SIGNAL( filterStart()	),
	     pkgList,	SLOT  ( clear()		) );

    connect( filter,	SIGNAL( filterStart()	),
	     this,	SLOT  ( busyCursor()		) );

    connect( filter,	SIGNAL( filterMatch( ZyppSel, ZyppPkg ) ),
	     pkgList,	SLOT  ( addPkgItem ( ZyppSel, ZyppPkg ) ) );

    connect( filter,	SIGNAL( filterFinished()       ),
	     pkgList,	SLOT  ( resort() ) );

    connect( filter,	SIGNAL( filterFinished()  ),
	     pkgList,	SLOT  ( selectSomething() ) );

    connect( filter,	SIGNAL( filterFinished()       ),
	     pkgList,	SLOT  ( logExcludeStatistics() ) );

    connect( filter,	SIGNAL( filterFinished()       ),
	     pkgList,	SLOT  ( maybeSetFocus() ) );

    connect( filter,	SIGNAL( filterFinished()       ),
	     this,	SLOT  ( normalCursor() ) );


    if ( hasUpdateSignal && _filters->diskUsageList() )
    {
	connect( filter,		SIGNAL( updatePackages()   ),
		 pkgList,		SLOT  ( updateItemStates() ) );

	if ( _filters->diskUsageList() )
	{
	    connect( filter,			SIGNAL( updatePackages()  ),
		     _filters->diskUsageList(),	SLOT  ( updateDiskUsage() ) );
	}
    }
}


void
YQPackageSelector::makeConnections()
{
    connectFilter( _updateProblemFilterView,	_pkgList, false );
    connectFilter( _patternList,		_pkgList );
    connectFilter( _langList,			_pkgList );
    connectFilter( _repoFilterView,		_pkgList, false );
    connectFilter( _serviceFilterView,		_pkgList, false );
    connectFilter( _pkgClassFilterView,		_pkgList, false );
    connectFilter( _statusFilterView,		_pkgList, false );
    connectFilter( _searchFilterView,		_pkgList, false );

    if ( _searchFilterView && _pkgList )
    {
	connect( _searchFilterView,	SIGNAL( message( const QString & ) ),
		 _pkgList,		SLOT  ( message( const QString & ) ) );
    }

    if ( _repoFilterView && _pkgList )
    {
	connect( _repoFilterView,	SIGNAL( filterNearMatch	 ( ZyppSel, ZyppPkg ) ),
		 _pkgList,		SLOT  ( addPkgItemDimmed ( ZyppSel, ZyppPkg ) ) );
    }

    if ( _serviceFilterView && _pkgList )
    {
	connect( _serviceFilterView,	SIGNAL( filterNearMatch	 ( ZyppSel, ZyppPkg ) ),
		 _pkgList,		SLOT  ( addPkgItemDimmed ( ZyppSel, ZyppPkg ) ) );
    }

    if ( _pkgList && _filters->diskUsageList() )
    {

	connect( _pkgList,			SIGNAL( statusChanged()	  ),
		 _filters->diskUsageList(),	SLOT  ( updateDiskUsage() ) );
    }

    connectPatchList();


    //
    // Connect package conflict dialog
    //

    if ( _pkgConflictDialog )
    {
	if (_pkgList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _pkgList,			SLOT  ( updateItemStates() ) );
	}

	if ( _patternList )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _patternList,		SLOT  ( updateItemStates() ) );
	}


	if ( _filters->diskUsageList() )
	{
	    connect( _pkgConflictDialog,	SIGNAL( updatePackages()   ),
		     _filters->diskUsageList(),	SLOT  ( updateDiskUsage()  ) );
	}
    }


    //
    // Connect package versions view
    //

    if ( _pkgVersionsView && _pkgList )
    {
	connect( _pkgVersionsView,	SIGNAL( candidateChanged( ZyppObj ) ),
		 _pkgList,		SLOT  ( updateItemData()    ) );

	connect( _pkgVersionsView,	SIGNAL( statusChanged()	 ),
		 _pkgList,		SLOT  ( updateItemData() ) );
    }


    //
    // Hotkey to enable "patches" filter view on the fly
    //

    QShortcut * accel = new QShortcut( Qt::Key_F2, this, SLOT( hotkeyInsertPatchFilterView() ) );
    CHECK_NEW( accel );


    //
    // Update actions just before opening menus
    //

    if ( _pkgMenu && _pkgList )
    {
	connect( _pkgMenu,	SIGNAL( aboutToShow()	),
		 _pkgList,	SLOT  ( updateActions() ) );
    }

    if ( _patchMenu && _patchList )
    {
	connect( _patchMenu,	SIGNAL( aboutToShow()	),
		 _patchList,	SLOT  ( updateActions() ) );
    }
}


void
YQPackageSelector::autoResolveDependencies()
{
    if ( _autoDependenciesAction && ! _autoDependenciesAction->isChecked() )
	return;

    resolveDependencies();
}


int
YQPackageSelector::manualResolvePackageDependencies()
{
    if ( ! _pkgConflictDialog )
    {
	logError() << "No package conflict dialog existing" << endl;
	return QDialog::Accepted;
    }

    busyCursor();
    int result = _pkgConflictDialog->solveAndShowConflicts();
    normalCursor();

#if DEPENDENCY_FEEDBACK_IF_OK

    if ( result == QDialog::Accepted )
    {
	QMessageBox::information( this, "",
				  _( "All package dependencies are OK." ),
				  QMessageBox::Ok );
    }
#endif

    return result;
}


void
YQPackageSelector::addPatchFilterView()
{
    if ( ! _patchFilterView )
    {
	_patchFilterView = new YQPkgPatchFilterView( this );
	CHECK_NEW( _patchFilterView );
	_filters->addPage( _( "P&atches" ), _patchFilterView, "patches" );

	_patchList = _patchFilterView->patchList();
	CHECK_PTR( _patchList );

	connectPatchList();
    }
}


void
YQPackageSelector::hotkeyInsertPatchFilterView()
{
    if ( ! _patchFilterView )
    {
	logInfo() << "Activating patches filter view" << endl;

	addPatchFilterView();
	connectPatchList();

	_filters->showPage( _patchFilterView );
	_pkgList->clear();
	_patchList->filter();
    }
    else
    {
	_filters->showPage( _patchFilterView );
    }
}


void
YQPackageSelector::connectPatchList()
{
    if ( _pkgList && _patchList )
    {
	connectFilter( _patchList, _pkgList );

	connect( _patchList, SIGNAL( filterMatch   ( const QString &, const QString &, FSize ) ),
		 _pkgList,   SLOT  ( addPassiveItem( const QString &, const QString &, FSize ) ) );

	connect( _patchList,		SIGNAL( statusChanged()			),
		 this,			SLOT  ( autoResolveDependencies()	) );

	if ( _pkgConflictDialog )
	{
	    connect( _pkgConflictDialog,SIGNAL( updatePackages()		),
		     _patchList,	SLOT  ( updateItemStates()		) );
	}

	connect( this,			SIGNAL( refresh()			),
		 _patchList,		SLOT  ( updateItemStates()		) );

    }
}


void
YQPackageSelector::pkgExport()
{
    QString filename = YQApplication::askForSaveFileName( QString( DEFAULT_EXPORT_FILE_NAME ),	// startsWith
							  QString( "*.xml;;*" ),		// filter
							  _( "Save Package List" ) );

    if ( ! filename.isEmpty() )
    {
	zypp::syscontent::Writer writer;
	const zypp::ResPool & pool = zypp::getZYpp()->pool();


	// The ZYPP obfuscated C++ contest proudly presents:

	for_each( pool.begin(), pool.end(),
		  boost::bind( &zypp::syscontent::Writer::addIf,
			       boost::ref( writer ),
			       boost::placeholders::_1 ) );

	// Yuck. What a mess.
	//
	// Does anybody seriously believe this kind of thing is easier to read,
	// let alone use? Get real. This is an argument in favour of all C++
	// haters. And it's one that is really hard to counter.
	//
	// -sh 2006-12-13

	try
	{
	    std::ofstream exportFile( toUTF8( filename ).c_str() );
	    exportFile.exceptions( std::ios_base::badbit | std::ios_base::failbit );
	    exportFile << writer;

	    logInfo() << "Package list exported to " << filename << endl;
	}
	catch ( std::exception & exception )
	{
	    logWarning() << "Error exporting package list to " << filename << endl;

	    // The export might have left over a partially written file.
	    // Try to delete it. Don't care if it doesn't exist and unlink() fails.
	    QFile::remove(filename);

	    // Post error popup
	    QMessageBox::warning( this,						// parent
				  _( "Error" ),					// caption
				  _( "Error exporting package list to %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  Qt::NoButton,					// button1
				  Qt::NoButton );				// button2
	}
    }
}


void
YQPackageSelector::pkgImport()
{
    QString filename =	QFileDialog::getOpenFileName( this, _( "Load Package List" ), DEFAULT_EXPORT_FILE_NAME,
						      "*.xml+;;*"// filter
						      );

    if ( ! filename.isEmpty() )
    {
	logInfo() << "Importing package list from " << filename << endl;

	try
	{
	    std::ifstream importFile( toUTF8( filename ).c_str() );
	    zypp::syscontent::Reader reader( importFile );

	    //
	    // Put reader contents into maps
	    //

	    typedef zypp::syscontent::Reader::Entry	ZyppReaderEntry;
	    typedef std::pair<string, ZyppReaderEntry>	ImportMapPair;

	    map<string, ZyppReaderEntry> importPkg;
	    map<string, ZyppReaderEntry> importPatterns;

	    for ( zypp::syscontent::Reader::const_iterator it = reader.begin();
		  it != reader.end();
		  ++ it )
	    {
		string kind = it->kind();

		if	( kind == "package" )	importPkg.insert     ( ImportMapPair( it->name(), *it ) );
		else if ( kind == "pattern" )	importPatterns.insert( ImportMapPair( it->name(), *it ) );
	    }

	    logDebug() << "Found "	  << importPkg.size()
		       <<" packages and " << importPatterns.size()
		       << " patterns in " << filename
		       << endl;


	    //
	    // Set status of all patterns and packages according to import map
	    //

	    for ( ZyppPoolIterator it = zyppPatternsBegin();
		  it != zyppPatternsEnd();
		  ++it )
	    {
		ZyppSel selectable = *it;
		importSelectable( *it, importPatterns.find( selectable->name() ) != importPatterns.end(), "pattern" );
	    }

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		ZyppSel selectable = *it;
		importSelectable( *it, importPkg.find( selectable->name() ) != importPkg.end(), "package" );
	    }


	    //
	    // Display result
	    //

	    emit refresh();

	    if ( _statusFilterView )
	    {
		// Switch to "Installation Summary" filter view

		_filters->showPage( _statusFilterView );
		_statusFilterView->filter();
	    }

	}
	catch ( const zypp::Exception & exception )
	{
	    logWarning() << "Error reading package list from " << filename << endl;

	    // Post error popup
	    QMessageBox::warning( this,						// parent
				  _( "Error" ),					// caption
				  _( "Error loading package list from %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }
}


void
YQPackageSelector::importSelectable( ZyppSel		selectable,
				     bool		isWanted,
				     const char *	kind )
{
    ZyppStatus oldStatus = selectable->status();
    ZyppStatus newStatus = oldStatus;

    if ( isWanted )
    {
	//
	// Make sure this selectable does not get installed
	//

	switch ( oldStatus )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_KeepInstalled:
	    case S_Protected:
	    case S_Update:
	    case S_AutoUpdate:
		newStatus = oldStatus;
		break;

	    case S_Del:
	    case S_AutoDel:
		newStatus = S_KeepInstalled;
		logDebug() << "Keeping " << kind << " " << selectable->name() << endl;
		break;

	    case S_NoInst:
	    case S_Taboo:

		if ( selectable->hasCandidateObj() )
		{
		    newStatus = S_Install;
		    logDebug() << "Adding " << kind << " " <<  selectable->name() << endl;
		}
		else
		{
		    logDebug() << "Can't add " << kind << " " << selectable->name()
			       << ": No candidate" << endl;
		}
		break;
	}
    }
    else // ! isWanted
    {
	//
	// Make sure this selectable does not get installed
	//

	switch ( oldStatus )
	{
	    case S_Install:
	    case S_AutoInstall:
	    case S_KeepInstalled:
	    case S_Protected:
	    case S_Update:
	    case S_AutoUpdate:
		newStatus = S_Del;
		logDebug() << "Deleting " << kind << " " << selectable->name() << endl;
		break;

	    case S_Del:
	    case S_AutoDel:
	    case S_NoInst:
	    case S_Taboo:
		newStatus = oldStatus;
		break;
	}
    }

    if ( oldStatus != newStatus )
	selectable->setStatus( newStatus );
}


void
YQPackageSelector::globalUpdatePkg( bool force )
{
    if ( ! _pkgList )
	return;

    int count = _pkgList->globalSetPkgStatus( S_Update, force,
					      true ); // countOnly
    logInfo() << count << " pkgs found for update" << endl;

    if ( count >= GLOBAL_UPDATE_CONFIRMATION_THRESHOLD )
    {
	if ( QMessageBox::question( this, "",	// caption
				    // Translators: %1 is the number of affected packages
				    _( "%1 packages will be updated" ).arg( count ),
				    _( "&Continue" ), _( "C&ancel" ),
				    0,		// defaultButtonNumber (from 0)
				    1 )		// escapeButtonNumber
	     == 1 )	// "Cancel"?
	{
	    return;
	}
    }

    (void) _pkgList->globalSetPkgStatus( S_Update, force,
					 false ); // countOnly

    if ( _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->clear();
	_statusFilterView->showTransactions();
	_statusFilterView->filter();
    }
}


void
YQPackageSelector::updateRepositoryUpgradeLabel()
{
    zypp::ResPool::repository_iterator it;
    _repoUpgradeLabel->setText("");
    _repoUpgradingLabel->setText("");

    // we iterate twice to show first the repo upgrades that
    // can be cancelled, and then the repo that can be added
    for ( it = zypp::getZYpp()->pool().knownRepositoriesBegin();
	  it != zypp::getZYpp()->pool().knownRepositoriesEnd();
	  ++it )
    {
	zypp::Repository repo(*it);
	// add the option to cancel the upgrade job against this
	// repository if there is a job for it
	if ( zypp::getZYpp()->resolver()->upgradingRepo(repo) )
	{
	    _repoUpgradingLabel->setText(_repoUpgradingLabel->text() + _("<p><small><a href=\"repoupgraderemove:///%1\">Cancel switching</a> system packages to versions in repository %2</small></p>")
					 .arg(fromUTF8(repo.alias().c_str()))
					 .arg(fromUTF8(repo.name().c_str()))
					 );
	}
    }

    for ( it = zypp::getZYpp()->pool().knownRepositoriesBegin();
	  it != zypp::getZYpp()->pool().knownRepositoriesEnd();
	  ++it )
    {
	zypp::Repository repo(*it);
	// add the option to upgrade to this repo packages if it is not the system
	// repository and there is no upgrade job in the solver for it
	// and the repo is the one selected right now
	if ( ! zypp::getZYpp()->resolver()->upgradingRepo(repo) &&
	     ! repo.isSystemRepo() &&
	     _repoFilterView->selectedRepo() == repo )
	{
	    _repoUpgradeLabel->setText(_repoUpgradeLabel->text() + _("<p><a href=\"repoupgradeadd:///%1\">Switch system packages</a> to the versions in this repository (%2)</p>")
				       .arg(fromUTF8(repo.alias().c_str()))
				       .arg(fromUTF8(repo.name().c_str()))
				       );
	}
    }
    _repoUpgradeLabel->setVisible(!_repoUpgradeLabel->text().isEmpty() &&
				  _repoFilterView->isVisible() );
    _repoUpgradingLabel->setVisible(!_repoUpgradingLabel->text().isEmpty());
}


void
YQPackageSelector::slotRepoUpgradeLabelLinkClicked(const QString &link)
{
    logDebug() << "link " << link << " clicked on label" << endl;

    QUrl url(link);
    if (url.scheme() == "repoupgradeadd")
    {
	logDebug() << "looking for repo " << url.path() << endl;
	std::string alias(url.path().remove(0,1).toStdString());
	zypp::Repository repo(zypp::getZYpp()->pool().reposFind(alias));
	logDebug() << repo.name() << endl;

	if ( repo != zypp::Repository::noRepository )
	{
	    zypp::getZYpp()->resolver()->addUpgradeRepo(repo);
	    // Do not complain about vendor change when switching repos (bsc##1149391)
	    zypp::getZYpp()->resolver()->dupSetAllowVendorChange(true);
	}
    }
    else if (url.scheme() == "repoupgraderemove")
    {
	std::string alias(url.path().remove(0,1).toStdString());
	zypp::Repository repo(zypp::getZYpp()->pool().reposFind(alias));

	if (  repo != zypp::Repository::noRepository )
	    zypp::getZYpp()->resolver()->removeUpgradeRepo(repo);
    }
    else
	logDebug() << "unknown link operation " << url.scheme() << endl;

    resolveDependencies();
    emit refresh();
}


void
YQPackageSelector::showProducts()
{
    YQPkgProductDialog::showProductDialog(this);
}


void
YQPackageSelector::showHistory()
{
    YQPkgHistoryDialog::showHistoryDialog(this);
}


void
YQPackageSelector::installDevelPkgs()
{
    installSubPkgs( "-devel" );
}


void
YQPackageSelector::installDebugInfoPkgs()
{
    installSubPkgs( "-debuginfo" );
}


void
YQPackageSelector::installDebugSourcePkgs()
{
    installSubPkgs( "-debugsource" );
}


void
YQPackageSelector::installRecommendedPkgs()
{
    zypp::getZYpp()->resolver()->setIgnoreAlreadyRecommended( false );
    resolveDependencies();

    if ( _filters && _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->filter();
    }

    YQPkgChangesDialog::showChangesDialog( this,
					   _( "Added Subpackages:" ),
					   _( "&OK" ),
					   QString(),			// rejectButtonLabel
					   YQPkgChangesDialog::FilterAutomatic,
					   YQPkgChangesDialog::OptionNone );	// showIfEmpty
}


void
YQPackageSelector::pkgExcludeDebugChanged( bool on )
{
    if ( _optionsMenu && _pkgList )
    {
	if ( _excludeDebugInfoPkgs )
	    _excludeDebugInfoPkgs->enable( ! on );

	_pkgList->applyExcludeRules();
    }
}


void
YQPackageSelector::pkgExcludeDevelChanged( bool on )
{
    if ( _optionsMenu && _pkgList )
    {
	if ( _excludeDevelPkgs )
	    _excludeDevelPkgs->enable( ! on );

	_pkgList->applyExcludeRules();
    }
}


void
YQPackageSelector::pkgVerifySytemModeChanged( bool on )
{
    zypp::getZYpp()->resolver()->setSystemVerification( on );
}


void
YQPackageSelector::pkgInstallRecommendedChanged( bool on )
{
    zypp::getZYpp()->resolver()->setOnlyRequires( !on );
    resolveDependencies();
}


void
YQPackageSelector::pkgCleanDepsOnRemoveChanged( bool on )
{
    zypp::getZYpp()->resolver()->setCleandepsOnRemove( on );
    resolveDependencies();
}


void
YQPackageSelector::pkgAllowVendorChangeChanged( bool on )
{
    zypp::getZYpp()->resolver()->setAllowVendorChange( on );
    zypp::getZYpp()->resolver()->dupSetAllowVendorChange( on ); // bsc#1170521
    resolveDependencies();
}


void
YQPackageSelector::installSubPkgs( const QString & suffix )
{
    // Find all matching packages and put them into a QMap

    QMap<QString, ZyppSel> subPkgs;

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	QString name = (*it)->name().c_str();

	if (  name.endsWith( suffix ) || name.endsWith( suffix + "-32bit" ) )
	{
	    subPkgs[ name ] = *it;

	    logDebug() << "Found subpackage: " << name << endl;
	}
    }


    // Now go through all packages and look if there is a corresponding subpackage in the QMap

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	QString name = (*it)->name().c_str();

	if ( subPkgs.contains( name + suffix ) )
	{
	    QString subPkgName( name + suffix );
	    ZyppSel subPkg = subPkgs[ subPkgName ];

	    switch ( (*it)->status() )
	    {
		case S_AutoDel:
		case S_NoInst:
		case S_Protected:
		case S_Taboo:
		case S_Del:
		    // Don't install the subpackage
		    logInfo() << "Ignoring unwanted subpackage " << subPkgName << endl;
		    break;

		case S_AutoInstall:
		case S_Install:
		case S_KeepInstalled:

		    // Install the subpackage, but don't try to update it

		    if ( ! subPkg->installedObj() )
		    {
			subPkg->setStatus( S_Install );
			logInfo() << "Installing subpackage " << subPkgName << endl;
		    }
		    break;


		case S_Update:
		case S_AutoUpdate:

		    // Install or update the subpackage

		    if ( ! subPkg->installedObj() )
		    {
			subPkg->setStatus( S_Install );
			logInfo() << "Installing subpackage " << subPkgName << endl;
		    }
		    else
		    {
			subPkg->setStatus( S_Update );
			logInfo() << "Updating subpackage " << subPkgName << endl;
		    }
		    break;

		    // Intentionally omitting 'default' branch so the compiler can
		    // catch unhandled enum states
	    }
	}
    }


    if ( _filters && _statusFilterView )
    {
	_filters->showPage( _statusFilterView );
	_statusFilterView->filter();
    }

    YQPkgChangesDialog::showChangesDialog( this,
					   _( "Added Subpackages:" ),
					   QRegExp( ".*" + suffix + "$" ),
					   _( "&OK" ),
					   QString(),			// rejectButtonLabel
					   YQPkgChangesDialog::FilterAutomatic,
					   YQPkgChangesDialog::OptionNone );	// showIfEmpty
}


bool
YQPackageSelector::anyRetractedPkgInstalled()
{
    logInfo() << "Checking for retracted installed packages..." << endl;

    for ( ZyppPoolIterator it = zyppPkgBegin(); it != zyppPkgEnd(); ++it )
    {
	if ( (*it)->hasRetractedInstalled() )
	    return true;
    }

    logInfo() << "No retracted packages installed." << endl;

    return false;
}


void
YQPackageSelector::loadSettings()
{
    QString settingsName = "YQPackageSelector";

    if ( onlineUpdateMode() )
	settingsName = "YQOnlineUpdate";

    if ( updateMode() )
	settingsName = "YQSystemUpdate";

    QSettings settings( QSettings::UserScope, SETTINGS_DIR, settingsName );

    _showDevelAction->setChecked(settings.value( "Options/showDevelPackages", true ).toBool());
    pkgExcludeDevelChanged(_showDevelAction->isChecked());

    _showDebugAction->setChecked(settings.value( "Options/showDebugPackages", true ).toBool());
    pkgExcludeDebugChanged(_showDebugAction->isChecked());

    loadCommonSettings();
}


void
YQPackageSelector::loadCommonSettings()
{
    map<string, string> sysconfig = zypp::base::sysconfig::read( PATH_TO_YAST_SYSCONFIG );

    bool auto_check = AUTO_CHECK_DEPENDENCIES_DEFAULT;
    auto it = sysconfig.find( OPTION_AUTO_CHECK );

    if ( it != sysconfig.end() )
	auto_check = it->second == "yes";

    _autoDependenciesAction->setChecked(auto_check);

    bool verify_system = zypp::getZYpp()->resolver()->systemVerification();
    it = sysconfig.find( OPTION_VERIFY );

    if ( it != sysconfig.end() )
	verify_system = it->second == "yes";

    _verifySystemModeAction->setChecked( verify_system );
    pkgVerifySytemModeChanged( verify_system );

    bool install_recommended = ! zypp::getZYpp()->resolver()->onlyRequires();
    it = sysconfig.find( OPTION_RECOMMENDED );

    if (it != sysconfig.end())
	install_recommended = it->second == "yes";

    _installRecommendedAction->setChecked( install_recommended );
    pkgInstallRecommendedChanged(install_recommended);

    bool allow_vendor_change = zypp::getZYpp()->resolver()->allowVendorChange();
    _allowVendorChangeAction->setChecked( allow_vendor_change );
    pkgAllowVendorChangeChanged( allow_vendor_change );

    bool clean_deps_on_remove = zypp::getZYpp()->resolver()->cleandepsOnRemove();
    _cleanDepsOnRemoveAction->setChecked( clean_deps_on_remove );
    pkgCleanDepsOnRemoveChanged( clean_deps_on_remove );
}


void
YQPackageSelector::saveSettings()
{
    QString settingsName = "YQPackageSelector";

    if ( onlineUpdateMode() )
	settingsName = "YQOnlineUpdate";

    if ( updateMode() )
	settingsName = "YQSystemUpdate";

    QSettings settings( QSettings::UserScope, SETTINGS_DIR, settingsName );

    settings.setValue("Options/showDevelPackages", _showDevelAction->isChecked() );
    settings.setValue("Options/showDebugPackages", _showDebugAction->isChecked() );

    saveCommonSettings();
}

void
YQPackageSelector::saveCommonSettings()
{
    try
    {
	zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
					       OPTION_AUTO_CHECK,
					       ( _autoDependenciesAction->isChecked() ? "yes" : "no" ),
					       "Automatic dependency checking");
	zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
					       OPTION_VERIFY,
					       ( _verifySystemModeAction->isChecked() ? "yes" : "no" ),
					       "System verification mode");
	zypp::base::sysconfig::writeStringVal( PATH_TO_YAST_SYSCONFIG,
					       OPTION_RECOMMENDED,
					       ( _installRecommendedAction->isChecked() ? "yes" : "no" ),
					       "Install recommended packages");
    }
    catch( const std::exception &e )
    {
	logError() << "Writing " << PATH_TO_YAST_SYSCONFIG << " failed" << endl;
    }
}

void YQPackageSelector::busyCursor()
{
    YQUI::ui()->busyCursor();
}

void YQPackageSelector::normalCursor()
{
    YQUI::ui()->normalCursor();
}


