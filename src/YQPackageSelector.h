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


#ifndef YQPackageSelector_h
#define YQPackageSelector_h

#include <QWidget>
#include <QColor>

#include "YQPackageSelectorBase.h"
#include "YQPkgObjList.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QListWidget;
class QProgressBar;
class QPushButton;
class QSplitter;
class QTabWidget;
class QMenu;
class QMenuBar;

class QY2ComboTabWidget;

class YQPkgChangeLogView;
class YQPkgDependenciesView;
class YQPkgDescriptionView;
class YQPkgFileListView;
class YQPkgFilterTab;
class YQPkgLangList;
class YQPkgList;
class YQPkgClassFilterView;
class YQPkgPatchFilterView;
class YQPkgPatchList;
class YQPkgPatternList;
class YQPkgRepoFilterView;
class YQPkgSearchFilterView;
class YQPkgSelList;
class YQPkgServiceFilterView;
class YQPkgStatusFilterView;
class YQPkgTechnicalDetailsView;
class YQPkgUpdateProblemFilterView;
class YQPkgVersionsView;

class YQPackageSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQPackageSelector( QWidget * parent, long modeFlags = 0 );
    ~YQPackageSelector();


public slots:

    /**
     * Resolve package dependencies manually.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int manualResolvePackageDependencies();

    /**
     * Automatically resolve package dependencies if desired
     * (if the "auto check" checkbox is on).
     **/
    void autoResolveDependencies();

#if FIXME_IMPORT_EXPORT
    /**
     * Export all current selection/package states
     **/
    void pkgExport();

    /**
     * Import selection/package states
     **/
    void pkgImport();
#endif

    /**
     * Install any -devel package for packages that are installed or marked for
     * installation
     **/
    void installDevelPkgs();

    /**
     * Install available -debuginfo packages for packages that are installed or
     * marked for installation
     **/
    void installDebugInfoPkgs();

    /**
     * Install available -debugsource packages for packages that are installed
     * or marked for installation
     **/
    void installDebugSourcePkgs();

    /**
     * Install recommended packages for packages that are installed
     **/
    void installRecommendedPkgs();

    /**
     * Install any subpackage that ends with 'suffix' for packages that are
     * installed or marked for installation
     **/
    void installSubPkgs( const QString & suffix );

    /**
     * Enable or disable the package exclude rules (show or suppress -debuginfo
     * or -devel packages) according to the current menu settings and apply the
     * exclude rules.
     **/
    void pkgExcludeDebugChanged( bool on );
    void pkgExcludeDevelChanged( bool on );


    /*
     * Enable or disable verify system mode of the solver
     */
    void pkgVerifySytemModeChanged( bool on );

    /*
     * Install recommended packages
     */
    void pkgInstallRecommendedChanged( bool on );

    /*
     * Enable or disable CleandepsOnRemove of the solver
     * (= Cleanup when deleting packages)
     */
    void pkgCleanDepsOnRemoveChanged( bool on );

    /*
     * Enable or disable vendor change allowed of the solver
     */
    void pkgAllowVendorChangeChanged( bool on );



    /**
     * Display (generic) online help.
     **/
    void help();

    /**
     * Display online help about symbols (package status icons).
     **/
    void symbolHelp();

    /**
     * Display online help about magic keys.
     **/
    void keyboardHelp();

    /**
     * hides or shows the repository upgrade message
     */
    void updateRepositoryUpgradeLabel();

    /**
     * loads settings for the checkboxes in the option menu
     */
    void loadSettings();

    /**
     * saves settings of the checkboxes in the option menu
     */
    void saveSettings();


signals:

    /**
     * Emitted once (!) when the dialog is about to be shown, when all widgets
     * are created and all signal/slot connections are set up - when it makes
     * sense to load data.
     **/
    void loadData();

    /**
     * Emitted when the internal data base might have changed and a refresh of
     * all displayed data might be necessary - e.g., when saved (exported) pkg
     * states are reimported.
     **/
    void refresh();


protected slots:

    /**
     * Add the "Patches" filter view, if it is not already there.
     **/
    void addPatchFilterView();

    /**
     * Add the "Patches" filter view upon hotkey (F2).
     **/
    void hotkeyInsertPatchFilterView();

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update", if there is
     * a candidate package that is newer than the installed package.
     **/
    void globalUpdatePkg() { globalUpdatePkg( false ); }

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update", even if the
     * candidate package is not newer than the installed package.
     **/
    void globalUpdatePkgForce() { globalUpdatePkg( true ); }

    /**
     * Show all products in a popup dialog.
     **/
    void showProducts();

    /**
     * Show dialog for pkgmgr history
     */
    void showHistory();

    /**
     * a link in the repo upgrade label was clicked
     */
    void slotRepoUpgradeLabelLinkClicked(const QString &link);

    /**
     * Show the busy cursor (clock)
     */
    void busyCursor();

    /**
     * Show the standard cursor (arrow)
     */
    void normalCursor();


protected:

    // Layout methods - create and layout widgets

    void basicLayout();

    QWidget *	layoutRightPane		( QWidget *parent );
    void	layoutFilters		( QWidget *parent );
    void 	layoutPkgList		( QWidget *parent );
    void 	layoutDetailsViews	( QWidget *parent );
    void 	layoutButtons		( QWidget *parent );
    void 	layoutMenuBar		( QWidget *parent );

    /**
     * Establish Qt signal / slot connections.
     *
     * This really needs to be a separate method to make sure all affected
     * wigets are created at this point.
     **/
    void makeConnections();

    /**
     * Add pulldown menus to the menu bar.
     *
     * This really needs to be a separate method to make sure all affected
     * wigets are created at this point.
     **/
    void addMenus();

    /**
     * Connect a filter view that provides the usual signals with a package
     * list. By convention, filter views provide the following signals:
     *	  filterStart()
     *	  filterMatch()
     *	  filterFinished()
     *	  updatePackages()  (optional)
     **/
    void connectFilter( QWidget *	filter,
			QWidget *	pkgList,
			bool		hasUpdateSignal = true );

    /**
     * Connect the patch list. Caution: Possible bootstrap problem!
     **/
    void connectPatchList();

    /**
     * Set the status of all installed packages (all in the pool, not only
     * those currently displayed in the package list) to "update" and switch to
     * the "Installation Summary" view afterwards.
     *
     * 'force' indicates if this should also be done if the the candidate
     * package is not newer than the installed package.
     **/
    void globalUpdatePkg( bool force );


#if FIXME_IMPORT_EXPORT
    /**
     * Import one selectable: Set its status according to 'isWanted'
     * based on its old status.
     * 'kind' is 'package' or 'pattern' (used only for debug logging).
     **/
    void importSelectable( ZyppSel	selectable,
			   bool		isWanted,
			   const char * kind );
#endif

    /**
     * Return 'true' if any selectable has any retracted package version
     * installed.
     **/
    bool anyRetractedPkgInstalled();

    /**
     * Return HTML code describing a symbol (an icon).
     **/
    QString symHelp( const QString & imgFileName,
		     const QString & summary,
		     const QString & explanation	);


    /**
     * Return HTML code describing a key.
     **/
    QString keyHelp( const QString & key,
		     const QString & summary,
		     const QString & explanation	);

    /**
     * loads settings that are shared with other frontends
     */
    void loadCommonSettings();

    /**
     * saves settings that are shared with other frontends
     */
    void saveCommonSettings();

    /**
     * Basic HTML formatting: Embed text into <p> ... </p>
     **/
    static QString para( const QString & text );

    /**
     * Basic HTML formatting: Embed text into <li> ... </li>
     **/
    static QString listItem( const QString & text );


    // Data members

    QAction *				_autoDependenciesAction;
    QPushButton *			_checkDependenciesButton;
    QTabWidget *			_detailsViews;
    YQPkgFilterTab *			_filters;
    YQPkgChangeLogView *		_pkgChangeLogView;
    YQPkgDependenciesView *		_pkgDependenciesView;
    YQPkgDescriptionView *		_pkgDescriptionView;
    YQPkgFileListView *			_pkgFileListView;
    QLabel *                            _repoUpgradeLabel;
    QLabel *                            _repoUpgradingLabel;
    QWidget *                           _notificationsContainer;
    YQPkgRepoFilterView *		_repoFilterView;
    YQPkgServiceFilterView *		_serviceFilterView;
    YQPkgLangList *			_langList;
    YQPkgList *				_pkgList;
    YQPkgPatternList *			_patternList;
    YQPkgClassFilterView *              _pkgClassFilterView;
    YQPkgSearchFilterView *		_searchFilterView;
    YQPkgStatusFilterView *		_statusFilterView;
    YQPkgTechnicalDetailsView *		_pkgTechnicalDetailsView;
    YQPkgUpdateProblemFilterView *	_updateProblemFilterView;
    YQPkgVersionsView *			_pkgVersionsView;
    YQPkgPatchFilterView *		_patchFilterView;
    YQPkgPatchList *			_patchList;

    QMenuBar *				_menuBar;
    QMenu *				_fileMenu;
    QMenu *				_pkgMenu;
    QMenu *				_patchMenu;
    QMenu *				_configMenu;
    QMenu *				_dependencyMenu;
    QMenu *				_optionsMenu;
    QMenu *				_extrasMenu;
    QMenu *				_helpMenu;

    QAction *                           _showDevelAction;
    QAction *                           _showDebugAction;
    QAction *                           _verifySystemModeAction;
    QAction *                           _installRecommendedAction;
    QAction *                           _cleanDepsOnRemoveAction;
    QAction *                           _allowVendorChangeAction;

    YQPkgObjList::ExcludeRule *		_excludeDevelPkgs;
    YQPkgObjList::ExcludeRule *		_excludeDebugInfoPkgs;

    QColor				_normalButtonBackground;
};



#endif // YQPackageSelector_h
