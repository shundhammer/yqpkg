/*
  Copyright (C) 2020 SUSE LLC
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

  File:	      YQMenuBar.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQMenuBar_h
#define YQMenuBar_h

#include <QMenuBar>
#include <yui/YMenuBar.h>
#include <QHash>

class QAction;
class QPushButton;
class QMenu;

class YQMenuBar : public QMenuBar, public YMenuBar
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQMenuBar( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQMenuBar();

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * Implemented from YMenuWidget.
     **/
    virtual void rebuildMenuTree();

    /**
     * Set enabled / disabled state.
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

    /**
     * Enable or disable an item.
     *
     * Reimplemented from YMenuWidget.
     **/
    virtual void setItemEnabled( YMenuItem * item, bool enabled );

    /**
     * Show or hide an item.
     *
     * Reimplemented from YMenuWidget.
     **/
    virtual void setItemVisible( YMenuItem * item, bool visible );

    /**
     * Activate the item selected in the tree. Can be used in tests to simulate
     * user input.
     **/
    virtual void activateItem( YMenuItem * item );

    /**
     * Notification that some shortcut was changed.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void shortcutChanged();

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


protected:

    /**
     * Recursively insert menu items into 'menu' from iterator 'begin' to
     * iterator 'end'.
     **/
    void rebuildMenuTree( QMenu * menu,
                          YItemIterator begin,
                          YItemIterator end );


    //
    // Data members
    //

    YMenuItem *                 _selectedItem;
    QMap<QAction *,YMenuItem *> _actionMap;
};

#endif // YQMenuBar_h
