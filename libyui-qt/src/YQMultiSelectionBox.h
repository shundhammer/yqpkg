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

  File:	      YQMultiSelectionBox.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQMultiSelectionBox_h
#define YQMultiSelectionBox_h

#include <QFrame>
#include <QTreeWidget>

#include <yui/YMultiSelectionBox.h>

class YQWidgetCaption;
class YQMultiSelectionBoxItem;


class YQMultiSelectionBox : public QFrame, public YMultiSelectionBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQMultiSelectionBox( YWidget * parent, const std::string & label );

    /**
     * Destructor.
     **/
    ~YQMultiSelectionBox();

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
     * Return the the item that currently has the keyboard focus
     * or 0 if no item currently has the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox.
     **/
    virtual YItem * currentItem();

    /**
     * Set the keyboard focus to the specified item.
     * 0 means clear the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox.
     **/
    virtual void setCurrentItem( YItem * item );

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
     **/
    virtual bool setKeyboardFocus();


signals:
    /**
     * Emitted upon when an item changes its state.
     **/
    void valueChanged();


protected slots:
    /**
     * Send SelectionChanged event if Opt(:notify) is set.
     **/
    void slotSelected();

    /**
     * Send ValueChanged event if Opt(:notify) is set.
     **/
    void slotValueChanged();

    void slotItemChanged( QTreeWidgetItem*, int );

protected:
    /**
     * Find the counterpart of 'item'. Return 0 if there is none.
     **/
    YQMultiSelectionBoxItem * findItem( YItem * item );


    YQWidgetCaption *	_caption;
    QTreeWidget * 	_qt_listView;
};



class YQMultiSelectionBoxItem: public QTreeWidgetItem
{
public:
    /**
     * Constructor.
     **/
    YQMultiSelectionBoxItem( YQMultiSelectionBox * 	parent,
			     QTreeWidget * 		listView,
			     YItem *			yItem );

    /**
     * Destructor.
     **/
    virtual ~YQMultiSelectionBoxItem() {}

    /**
     * Return the corresponding YItem.
     **/
    YItem * yItem() const { return _yItem; }


protected:

    //
    // Data members
    //

    YItem *			_yItem;
    int 			_serial;
    YQMultiSelectionBox * 	_multiSelectionBox;


    /**
     * The next serial number to use
     **/
    static int _item_count;
};

#endif // YQLabel_h
