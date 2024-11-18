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
  File:	      YQPkgPatchList.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgPatchList_h
#define YQPkgPatchList_h

#include <string>
#include "YQPkgObjList.h"
#include "YQPkgSelMapper.h"
#include <QTreeWidgetItem>
#include <QEvent>
#include <QMenu>

#define ENABLE_DELETING_PATCHES	1

using std::string;

class YQPkgPatchListItem;
class YQPkgPatchCategoryItem;

enum YQPkgPatchCategory	// This is also the sort order
{
    YQPkgYaSTPatch,
    YQPkgSecurityPatch,
    YQPkgRecommendedPatch,
    YQPkgOptionalPatch,
    YQPkgDocumentPatch,

    YQPkgUnknownPatchCategory = 9999
};



/**
 * @short Display a list of zypp::Patch objects.
 **/
class YQPkgPatchList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgPatchList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchList();


    enum FilterCriteria
    {
        RelevantPatches,		// needed (or broken)
        RelevantAndInstalledPatches,	// unneeded
        AllPatches			// all
    };


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();

    /**
     * Add a patch to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPatchItem( ZyppSel   selectable,
		       ZyppPatch zyppPatch );

    /**
     * Fill the patch list according to filterCriteria().
     **/
    void fillList();

    /**
     * Display a one-line message in the list.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void message( const QString & text );


public:

    /**
     * Set the filter criteria for fillList().
     **/
    void setFilterCriteria( FilterCriteria filterCriteria );

    /**
     * Returns the current filter criteria.
     **/
    FilterCriteria filterCriteria() const { return _filterCriteria; }

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgPatchListItem * selection() const;

    /**
     * Returns the column for the patch category
     **/
    int categoryCol() const { return _categoryCol; }

    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QMenu * addAllInListSubMenu( QMenu * menu );

    /**
     * Delayed initialization after the dialog is fully created.
     *
     * Reimplemented from QWidget.
     **/
    virtual void polish();


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted during filtering for non-pkg items:
     * pre-script, post-script, files
     **/
    void filterMatch( const QString & 	name,
		      const QString & 	summary,
		      FSize 		size );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:
   /** 
     * returns or creates a category item for a defined category
     */
    YQPkgPatchCategoryItem * category( YQPkgPatchCategory category );

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
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Reimplemented from YQPkgObjList / QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    int			_categoryCol;
    YQPkgSelMapper	_selMapper;
    FilterCriteria	_filterCriteria;
    QMap<YQPkgPatchCategory, YQPkgPatchCategoryItem*> _categories;
};



class YQPkgPatchListItem: public YQPkgObjListItem
{
public:

      /**
     * Constructor. Creates a YQPkgPatchList item that corresponds to
     * zyppPatch.
     **/
    YQPkgPatchListItem( YQPkgPatchList * 	patchList,
			   ZyppSel		selectable,
			   ZyppPatch 		zyppPatch );


    /**
     * Constructor. Creates a YQPkgPatchList item that corresponds to
     * zyppPatch.
     **/
    YQPkgPatchListItem( YQPkgPatchList * 	patchList,
                        YQPkgPatchCategoryItem *	parentCategory,
                        ZyppSel		selectable,
                        ZyppPatch 		zyppPatch );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchListItem();

    /**
     * Returns the original zyppPatch object.
     **/
    ZyppPatch zyppPatch() const { return _zyppPatch; }

    /**
     * Cycle the package status to the next valid value.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void cycleStatus();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QString toolTip( int column );

    /**
     * sorting function
     */
    virtual bool operator< ( const QTreeWidgetItem & other ) const;

    // Columns

    int statusCol()	const	{ return _patchList->statusCol();	}
    int summaryCol()	const	{ return _patchList->summaryCol();	}
    int categoryCol()	const	{ return _patchList->categoryCol();	}

    /**
     * Returns the category of this patch (security, recommended, ...).
     **/
    YQPkgPatchCategory patchCategory() const { return _patchCategory; }

protected:
    void init();
    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all patches.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgPatchList *	_patchList;
    ZyppPatch		_zyppPatch;
    YQPkgPatchCategory	_patchCategory;
};

class YQPkgPatchCategoryItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgPatchCategoryItem( YQPkgPatchCategory category, YQPkgPatchList *	patternList );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchCategoryItem();

    /**
     * Returns the first pattern. This should be the first in sort order.
     **/
    ZyppPatch firstPatch() const { return _firstPatch; }

    /**
     * Add a pattern to this category. This method sets firstPatch() if necessary.
     **/
    void addPatch( ZyppPatch patch );

    /**
     * sorting function
     */
    virtual bool operator< ( const QTreeWidgetItem & other ) const;

    /*
     * Open or close this subtree
     *
     * Reimplemented from QListViewItem to force categories open at all times
     **/
    virtual void setExpanded( bool open );
    
    /**
     * Maps a string patch category to the corresponding enum.
     **/
    static YQPkgPatchCategory patchCategory( QString category );
    static YQPkgPatchCategory patchCategory( const string & category );
    /**
     * Converts a patch category to a user-readable (translated) string.
     **/
    static QString asString( YQPkgPatchCategory category );

   
    /**
     * Category (and order ) for this item
     */
    YQPkgPatchCategory category() const { return _category; }


protected:

    /**
     * Set a suitable tree open/close icon depending on this category's
     * open/close status.
     *
     * The default QListView plus/minus icons would require treeStepSize() to
     * be set >0 and rootItemDecorated( true ), but that would look very ugly
     * in this context, so the pattern categories paint their own tree open /
     * close icons.
     **/
    void setTreeIcon( void );
    //
    // Data members
    //

    YQPkgPatchCategory _category;
    YQPkgPatchList *	_patchList;
    ZyppPatch		_firstPatch;
};


#endif // ifndef YQPkgPatchList_h
