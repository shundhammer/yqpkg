/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2025 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef InitReposPage_h
#define InitReposPage_h


#include <QPixmap>
#include <QWidget>

#include <zypp/RepoInfo.h>

class YQPkgRepoManager;


// Generated with 'uic' from a Qt designer .ui form: init-repos-page.ui
//
// Check out ../build/src/yqpkg_autogen/include/ui_init-repos-page.h
// for the variable names of the widgets.

#include "ui_init-repos-page.h"


/**
 * Workflow page to visualize what the YQPkgRepoManager is doing during the
 * "init repos" phase.
 **/
class InitReposPage: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    InitReposPage( YQPkgRepoManager * repoManager, QWidget * parent = 0 );

    /**
     * Destructor.
     **/
    ~InitReposPage();


public slots:

    /**
     * Reset the internal status and all widgets.
     **/
    void reset();

    /**
     * Notification that an enabled repo was found.
     **/
    void foundRepo( const zypp::RepoInfo & repo );

    /**
     * Notification that refreshing a repo starts.
     **/
    void refreshRepoStart( const zypp::RepoInfo & repo );

    /**
     * Notification that refreshing a repo is done.
     **/
    void refreshRepoDone ( const zypp::RepoInfo & repo );


protected:

    /**
     * Set up Qt signal / slot connections.
     **/
    void connectSignals();

    /**
     * Load the icons for this page.
     **/
    void loadIcons();

    /**
     * Set the icon for the list widget item for 'repo' to 'icon' and return
     * the item (or 0 if not found).
     **/
    QListWidgetItem * setItemIcon( const zypp::RepoInfo & repo,
                                   const QPixmap &        icon );

    /**
     * Return the item for the specified repo in the repos list widget
     * or 0 if there is no such item.
     **/
    QListWidgetItem * findRepoItem( const zypp::RepoInfo & repo );


    //
    // Data members
    //

    YQPkgRepoManager *  _repoManager;
    Ui::InitReposPage * _ui;       // See ui_init-repos-page.h

    int                 _reposCount;
    int                 _refreshDoneCount;

    QPixmap             _emptyIcon;
    QPixmap             _downloadOngoingIcon;
    QPixmap             _downloadDoneIcon;
};


#endif // InitReposPage_h
