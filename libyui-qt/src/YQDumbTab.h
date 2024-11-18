/*
  Copyright (C) 2000-2012 Novell, Inc
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


/*-/

  File:	      YQDumbTab.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQDumbTab_h
#define YQDumbTab_h

#include <QTabWidget>
#include <yui/YDumbTab.h>


class QTabBar;
class QChildEvent;


class YQDumbTab : public QTabWidget, public YDumbTab
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQDumbTab( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQDumbTab();

    /**
     * Add an item (a tab page).
     *
     * Reimplemented from YDumbTab.
     **/
    virtual void addItem( YItem * item );

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items (all tab pages).
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Notification that some shortcut was changed.
     *
     * Reimplemented from YDumbTab.
     **/
    virtual void shortcutChanged();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

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
    * Activate selected tab. Can be used in tests to simulate user input.
    *
    * Derived classes are required to implement this.
    **/
    virtual void activate();


public slots:

    /**
     * Send an event that the tab with the specified index is selected.
     **/
    void slotSelected( int index );


protected:

    /**
     * Notification that a child has been added or removed.
     *
     * Reimplemented from QObject.
     **/
    void childEvent( QChildEvent * event ) override;


    // Data members

    QTabBar *  _tabBar;
    QWidget *  _firstPage;
};

#endif // YQDumbTab_h
