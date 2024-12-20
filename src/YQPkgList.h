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


#ifndef YQPkgList_h
#define YQPkgList_h

#include "YQPkgObjList.h"
#include <QMenu>
#include <QResizeEvent>

class YQPkgListItem;


/**
 * Display a list of zypp::Package objects.
 **/
class YQPkgList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgList();


    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QMenu * addAllInListSubMenu( QMenu * menu );

    /**
     * Returns 'true' if there are any installed packages.
     **/
    static bool haveInstalledPkgs();

    /**
     * Set the status of all packages in the pool to a new value.
     * This is not restricted to the current content of this package list.
     * All selectables in the ZYPP pool are affected.
     *
     * 'force' indicates if it should be done even if it is not very useful,
     * e.g., if packages should be updated even if there is no newer version.
     *
     * If 'countOnly' is 'true', the status is not actually changed, only the
     * number of packages that would be affected is return.
     *
     * Return value: The number of status changes
     **/
    int globalSetPkgStatus( ZyppStatus newStatus, bool force, bool countOnly );


public slots:

    /**
     * Add a pkg to the list. Connect a filter's filterMatch() signal to this
     * slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgItem( ZyppSel selectable,
                     ZyppPkg zyppPkg );

    /**
     * Add a pkg to the list, but display it dimmed (grey text foreground
     * rather than normal black).
     **/
    void addPkgItemDimmed( ZyppSel selectable,
                           ZyppPkg zyppPkg );

    /**
     * Add a pkg to the list
     **/
    void addPkgItem( ZyppSel selectable,
                     ZyppPkg zyppPkg,
                     bool    dimmed );


    /**
     * Reimplemented from QListView / QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

#if 0
    /**
     * Ask for a file name and save the current pkg list to file.
     **/
    void askExportList() const;
#endif


    // No separate currentItemChanged( ZyppPkg ) signal:
    //
    // Use YQPkgObjList::currentItemChanged( ZyppObj ) instead and dynamic_cast
    // to ZyppPkg if required.  This saves duplicating a lot of code.

    /**
     * Clears the tree-widgets content, resets the optimal column width values
     *
     * Reimplemented from QPkgObjList, calls QPkgObjList::reset()
     **/
    virtual void clear() override;

    /**
     * Sort the tree widget again according to the column selected and
     * its current sort order.
     **/
    void resort();


protected:

    /**
     * Create the context menu for items that are not installed.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createNotInstalledContextMenu();

    /**
     * Create the context menu for installed items.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createInstalledContextMenu();

    /**
     * Resets the optimal column width values.
     * Needed for empty list.
     **/
    void resetOptimalColumnWidthValues();

    /**
     * Set and save optimal column widths depending on content only.
     *
     * There is currently no way to get the optimal widths without setting
     * them, so we have to do it.
     **/
    void updateOptimalColumnWidthValues(ZyppSel selectable, ZyppPkg zyppPkg);

    /**
     * Optimizes the column widths depending on content and the available
     * horizontal space.
     **/
    void optimizeColumnWidths();

    /**
     * Handler for resize events.
     * Triggers column width optimization.
     **/
    void resizeEvent(QResizeEvent *event);


    //
    // Data members
    //

    int _optimalColWidth_statusIcon;
    int _optimalColWidth_name;
    int _optimalColWidth_summary;
    int _optimalColWidth_version;
    int _optimalColWidth_instVersion;
    int _optimalColWidth_size;
};



class YQPkgListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgListItem( YQPkgList *  pkgList,
                   ZyppSel      selectable,
                   ZyppPkg      zyppPkg );

    /**
     * Destructor
     **/
    virtual ~YQPkgListItem();

    /**
     * Returns the parent package list.
     **/
    YQPkgList * pkgList() { return _pkgList; }

    /**
     * Returns the original object within the package manager backend.
     **/
    ZyppPkg zyppPkg() const { return _zyppPkg; }

    /**
     * Update this item's data completely.
     * Triggered by QY2ListView::updateAllItemData().
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void updateData();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QString toolTip( int column );

    /**
     * Returns true if this package is to be displayed dimmed,
     * i.e. with grey text foreground rather than the normal black.
     **/
    bool isDimmed() const { return _dimmed; }

    /**
     * Set the 'dimmed' flag.
     **/
    void setDimmed( bool d = true ) { _dimmed = d; }


protected:

    YQPkgList * _pkgList;
    ZyppPkg     _zyppPkg;
    bool        _dimmed;
};


#endif // ifndef YQPkgList_h
