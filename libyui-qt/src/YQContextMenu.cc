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

  File:	      YQContextMenu.cc

  Author:     Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


#include <QMenu>
#include <qtimer.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQContextMenu.h"
#include <yui/YEvent.h>

using std::endl;



YQContextMenu::YQContextMenu()
    : QObject ()
    , YContextMenu( )
    , _suppressCancelEvent(false )
    , _parent(0)
{
    yuiWarning() << "YQContextMenu";

}

YQContextMenu::YQContextMenu( QWidget* parent, const QPoint position )
    : QObject ()
    , YContextMenu(  )
    , _position ( position )
    , _parent(parent)
{
    // NOP
}


YQContextMenu::~YQContextMenu()
{
    // NOP
}


void
YQContextMenu::rebuildMenuTree()
{
    QMenu * menu = new QMenu( _parent );
    YUI_CHECK_NEW( menu );
    menu->setProperty( "class", "ycontextmenu QMenu" );

    connect( menu,	&pclass(menu)::triggered,
	     this,	&pclass(this)::menuEntryActivated );

    connect( menu,	&pclass(menu)::aboutToHide,
	     this,	&pclass(this)::slotMenuHidden );
    //
    // Recursively add Qt menu items from the YMenuItems
    //

    rebuildMenuTree( menu, itemsBegin(), itemsEnd() );
    menu->popup( _position );
}


void
YQContextMenu::rebuildMenuTree( QMenu * parentMenu, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it != end; ++it )
    {
	YItem * item = *it;
	QIcon icon;

	if ( item->hasIconName() )
	{
            icon = YQUI::ui()->loadIcon( item->iconName() );
	}

	if ( item->hasChildren() )
	{
	    QMenu * subMenu;

	    if ( icon.isNull() )
		subMenu = parentMenu->addMenu( fromUTF8( item->label() ));
	    else
		subMenu = parentMenu->addMenu( QIcon( icon ), fromUTF8( item->label() ));

	    connect( subMenu,	&pclass(subMenu)::triggered,
		     this,	&pclass(this)::menuEntryActivated );

	    rebuildMenuTree( subMenu, item->childrenBegin(), item->childrenEnd() );
	}
	else // No children - leaf entry
	{
	    // item->index() is guaranteed to be unique within this YContextMenu's items,
	    // so it can easily be used as unique ID in all Q3PopupMenus that belong
	    // to this YQContextMenu.

            QAction *act;

	    if ( icon.isNull() )
		act = parentMenu->addAction( fromUTF8( item->label() ) );
	    else
		act = parentMenu->addAction( QIcon( icon ), fromUTF8( item->label() ) );

            _serials[act] = item->index();
	}
    }
}

void
YQContextMenu::slotMenuHidden()
{
    // dirty hack
    // see menuEntryActivated() for details
    QTimer::singleShot( 150, this, SLOT( slotReturnMenuHidden() ) );
}


void
YQContextMenu::slotReturnMenuHidden()
{
    if ( ! _suppressCancelEvent )
        YQUI::ui()->sendEvent( new YCancelEvent() );

    _suppressCancelEvent = false;
}


void
YQContextMenu::menuEntryActivated( QAction* action )
{
    int serialNo = -1;
    if ( _serials.contains( action ) )
        serialNo = _serials[action];

    // yuiDebug() << "Selected menu entry #" << menu_item_index << endl;
    _selectedItem = findMenuItem( serialNo );

    if ( _selectedItem )
    {
	/*
	 * Defer the real returnNow() until all popup related events have been
	 * processed. This took me some hours to figure out; obviously
	 * exit_loop() doesn't have any effect as long as there are still
	 * popups open. So be it - use a zero timer to perform the real
	 * returnNow() later.
	 */

	/*
	 * The 100 delay is an ugly dirty workaround.
	 */
	_suppressCancelEvent = true;
	QTimer::singleShot( 100, this, SLOT( returnNow() ) );
    }
    else
    {
	yuiError() << "No menu item with serial no. " << serialNo << endl;
    }
}


void
YQContextMenu::returnNow()
{
    if ( _selectedItem )
    {
	YQUI::ui()->sendEvent( new YMenuEvent( _selectedItem ) );
	_selectedItem = 0;
    }
}


int YQContextMenu::preferredWidth()
{
    return 42;
}


int YQContextMenu::preferredHeight()
{
    return 42;
}


void
YQContextMenu::setSize( int newWidth, int newHeight )
{

}
