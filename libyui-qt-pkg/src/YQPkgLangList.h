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
  File:	      YQPkgLangList.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgLangList_h
#define YQPkgLangList_h

#include "YQPkgObjList.h"


class YQPkgLangListItem;

/**
 * @short Display a list of zypp::Selection objects.
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
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

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
    YQPkgLangListItem( YQPkgLangList *	    pkgSelList,
                       const zypp::Locale & lang );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    zypp::Locale zyppLang() const	{ return _zyppLang; }

    /// overloaded
    virtual void init();

    // Columns

    int statusCol()	const	{ return _langList->statusCol(); }

    /**
     * override this two as we don't have a real selectable and
     * the status depends on the language
     **/
    virtual ZyppStatus status() const;
    virtual void setStatus( ZyppStatus newStatus, bool sendSignals = true );

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

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all languages.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgLangList *	_langList;
    zypp::Locale	_zyppLang;
};


#endif // ifndef YQPkgLangList_h
