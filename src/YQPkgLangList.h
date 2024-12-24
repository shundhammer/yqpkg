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


#ifndef YQPkgLangList_h
#define YQPkgLangList_h

#include "YQPkgObjList.h"


class YQPkgLangListItem;

/**
 * Display a list of languages and locales.
 **/
class YQPkgLangList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgLangList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangList();


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
     * Add a selection to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addLangItem( const zypp::Locale & lang );

    /**
     * update from base class to not access selectables
     **/
    virtual void updateActions( YQPkgObjListItem * item = 0);

public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgLangListItem * selection() const;


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel selectable,
                      ZyppPkg pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the language list.
     **/
    void fillList();
};



class YQPkgLangListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgLangList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgLangListItem( YQPkgLangList *      pkgSelList,
                       const zypp::Locale & lang );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    zypp::Locale zyppLang() const { return _zyppLang; }


    /**
     * Return the column number (0..n) of the status column.
     **/
    int statusCol() const { return _langList->statusCol(); }

    /**
     * override this as we don't have a real selectable and
     * the status depends on the language.
     **/
    virtual ZyppStatus status() const;

    /**
     * Set the status of this item.
     **/
    virtual void setStatus( ZyppStatus newStatus,
                            bool       sendSignals = true );

    /**
     * Returns 'true' if this selectable's status is set by a selection
     * (rather than by the user or by the dependency solver).
     **/
    virtual bool bySelection() const;

    /**
     * Cycle the package status to the next valid value.
     **/
    virtual void cycleStatus();

    /**
     * Sorting function. Redefined here (bnc#428355) as the one from
     * parent YQPkgObjList can't be used - it refers to zyppObj's which
     * are NULL here as zypp::Locale is not zypp::ResObject anymore
     **/
    virtual bool operator< ( const QTreeWidgetItem & other ) const;

protected:

    void init();

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all languages.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgLangList * _langList;
    zypp::Locale    _zyppLang;
};


#endif // ifndef YQPkgLangList_h
