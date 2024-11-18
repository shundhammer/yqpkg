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

  File:	      YQSelectionBox.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQSelectionBox_h
#define YQSelectionBox_h

#include <QFrame>
#include <QTimer>

#include <yui/YSelectionBox.h>

class YQWidgetCaption;
class QListWidget;
class QListWidgetItem;


class YQSelectionBox : public QFrame, public YSelectionBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQSelectionBox( YWidget * parent, const std::string & label );

    /**
     * Destructor.
     **/
    virtual ~YQSelectionBox();

    /**
     * Change the label text.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Add an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Add multiple items.
     *
     * Reimplemented for efficiency from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

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
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

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
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Event filter.
     *
     * Reimplemented from QWidget.
     **/
    virtual bool eventFilter( QObject * obj, QEvent * ev );


protected slots:

    /**
     * Notification that an item has been selected.
     * This is only relevant if Opt( :notify ) is set.
     **/
    void slotSelectionChanged();

    /**
     * Notification that an item has been activated (double clicked).
     **/
    void slotActivated( QListWidgetItem * item );

    /**
     * Return after some millseconds delay - collect multiple events.
     * This is only relevant if Opt( :notify ) is set.
     **/
    void returnDelayed();

    /**
     * Return immediately.
     * This is only relevant if Opt( :notify ) is set.
     **/
    void returnImmediately();


protected:

    /**
     * Select an item by index.
     **/
    void selectItem( int index );

    /**
     * Internal addItem() method that will not do expensive operations in batch
     * mode.
     **/
    void addItem( YItem * item, bool batchMode );


    //
    // Data members
    //

    YQWidgetCaption *	_caption;
    QListWidget *	_qt_listWidget;
    QTimer 		_timer;
};

#endif // YQLabel_h
