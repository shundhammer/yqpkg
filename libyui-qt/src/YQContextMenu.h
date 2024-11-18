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

  File:	      YQContextMenu.h

  Author:     Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


#ifndef YQContextMenu_h
#define YQContextMenu_h

#include <qwidget.h>
#include <yui/YContextMenu.h>
#include <QHash>

class QAction;
class QPushButton;
class QMenu;

class YQContextMenu : public QObject,  public YContextMenu
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQContextMenu( );
    YQContextMenu( QWidget* parent, const QPoint position );


    /**
     * Destructor.
     **/
    virtual ~YQContextMenu();

    /**
     * Change the label on the button.
     *
     * Reimplemented from YContextMenu / YSelectionWidget.
     **/
//    virtual void setLabel( const std::string & label );

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * Implemented from YContextMenu.
     **/
    virtual void rebuildMenuTree();

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


protected slots:

    /**
     * Triggered when any menu item is activated.
     **/
    void menuEntryActivated( QAction * menuItem );

    /**
     * Triggered via menuEntryActivated() by zero timer to get back in sync
     * with the Qt event loop.
     **/
    void returnNow();

    /**
     * Triggered when the context menu is hidden
     **/
    void slotMenuHidden();

    /**
     * Triggered via slotMenuHidden() by zero timer to get back in sync
     * with the Qt event loop.
     **/
    void slotReturnMenuHidden();




protected:

    /**
     * Recursively insert menu items into 'menu' from iterator 'begin' to iterator 'end'.
     **/
    void rebuildMenuTree( QMenu * menu, YItemIterator begin, YItemIterator end );


    //
    // Data members
    //

    YMenuItem *		_selectedItem;
    QPushButton * 	_qt_button;
    QHash<QAction*,int> _serials;


private:
    QPoint 		_position;
    bool		_suppressCancelEvent;
    QWidget*		_parent;

};

#endif // YQContextMenu_h
