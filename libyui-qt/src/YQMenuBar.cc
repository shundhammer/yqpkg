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

  File:	      YQMenuBar.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QMenu>
#include <QTimer>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQMenuBar.h"
#include "YQSignalBlocker.h"
#include <yui/YEvent.h>

using std::string;



YQMenuBar::YQMenuBar( YWidget * parent )
    : QMenuBar( (QWidget *) parent->widgetRep() )
    , YMenuBar( parent )
    , _selectedItem( 0 )
{
    setWidgetRep( this );
}


YQMenuBar::~YQMenuBar()
{
    // NOP
}


void
YQMenuBar::rebuildMenuTree()
{
    //
    // Delete any previous menus
    // (in case the menu items got replaced)
    //

    QMenuBar::clear();
    _actionMap.clear();
    _selectedItem = 0;


    //
    // Create toplevel menus
    //

    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
	YUI_CHECK_PTR( item );

	if ( ! item->isMenu() )
	    YUI_THROW( YUIException( "YQMenuBar: Only menus allowed on toplevel." ) );

	QMenu * menu = QMenuBar::addMenu( fromUTF8( item->label() ));
	item->setUiItem( menu );
        menu->setEnabled( item->isEnabled() );
        menu->menuAction()->setVisible( item->isVisible() );

	connect( menu, &pclass(menu)::triggered,
                 this, &pclass(this)::menuEntryActivated );

	// Recursively add menu content
	rebuildMenuTree( menu, item->childrenBegin(), item->childrenEnd() );
    }
}


void
YQMenuBar::rebuildMenuTree( QMenu * parentMenu, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
	YUI_CHECK_PTR( item );
	QIcon icon;

	if ( item->hasIconName() )
	    icon = YQUI::ui()->loadIcon( item->iconName() );

	if ( item->isSeparator() )
	{
	    parentMenu->addSeparator();
	}
	else if ( item->isMenu() )
	{
	    QMenu * subMenu = parentMenu->addMenu( fromUTF8( item->label() ));
	    item->setUiItem( subMenu );
            subMenu->setEnabled( item->isEnabled() );
            // Do NOT call
            //   subMenu->setVisible( item->isVisible() );
            // here since this would make this pop-up menu visible immediately!
            // It has to wait until the user explicitly opens it.


	    if ( ! icon.isNull() )
		subMenu->setIcon( icon );

	    connect( subMenu,	&pclass(subMenu)::triggered,
		     this,	&pclass(this)::menuEntryActivated );

	    rebuildMenuTree( subMenu, item->childrenBegin(), item->childrenEnd() );
	}
	else // Plain menu item (leaf item)
	{
	    QAction * action = parentMenu->addAction( fromUTF8( item->label() ) );
	    item->setUiItem( action );
	    _actionMap[ action ] = item;
            action->setEnabled( item->isEnabled() );
            action->setVisible( item->isVisible() );

	    if ( ! icon.isNull() )
		action->setIcon( icon );
	}
    }
}


void
YQMenuBar::menuEntryActivated( QAction * action )
{
    if ( _actionMap.contains( action ) )
        _selectedItem = _actionMap[ action ];

    if ( _selectedItem )
    {
	// yuiDebug() << "Selected menu entry \"" << fromUTF8( _selectedItem->label() ) << "\"" << endl;

	/*
	 * Defer the real returnNow() until all popup related events have been
	 * processed. This took me some hours to figure out; obviously
	 * exit_loop() doesn't have any effect as long as there are still
	 * popups open. So be it - use a timer to perform the real returnNow()
	 * later.
	 */

	/*
	 * The 100 delay is an ugly dirty workaround.
	 */
	QTimer::singleShot( 100, this, SLOT( returnNow() ) );
    }
    else
    {
	yuiError() << "Unknown action \"" << action->text() << "\"" << endl;
    }
}


void
YQMenuBar::returnNow()
{
    if ( _selectedItem )
    {
	YQUI::ui()->sendEvent( new YMenuEvent( _selectedItem ) );
	_selectedItem = 0;
    }
}


void
YQMenuBar::setItemEnabled( YMenuItem * item, bool enabled )
{
    QObject * qObj = static_cast<QObject *>( item->uiItem() );

    if ( qObj )
    {
	QMenu * menu = qobject_cast<QMenu *>( qObj );

	if ( menu )
	    menu->setEnabled( enabled );
	else
	{
	    QAction * action = qobject_cast<QAction *>( qObj );

	    if ( action )
		action->setEnabled( enabled );
	}
    }

    YMenuWidget::setItemEnabled( item, enabled );
}


void
YQMenuBar::setItemVisible( YMenuItem * item, bool visible )
{
    QObject * qObj = static_cast<QObject *>( item->uiItem() );

    if ( qObj )
    {
	QMenu * menu = qobject_cast<QMenu *>( qObj );

	if ( menu )
	    menu->menuAction()->setVisible( visible );
	else
	{
	    QAction * action = qobject_cast<QAction *>( qObj );

	    if ( action )
		action->setVisible( visible );
	}
    }

    YMenuWidget::setItemVisible( item, visible );
}


void
YQMenuBar::setEnabled( bool enabled )
{
    YWidget::setEnabled( enabled );
}


int YQMenuBar::preferredWidth()
{
    return sizeHint().width();
}


int YQMenuBar::preferredHeight()
{
    return sizeHint().height();
}


void
YQMenuBar::setSize( int newWidth, int newHeight )
{
    QWidget::resize( newWidth, newHeight );
}


bool
YQMenuBar::setKeyboardFocus()
{
    QWidget::setFocus();

    return true;
}


void
YQMenuBar::activateItem( YMenuItem * item )
{
    if ( item )
	YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}


void
YQMenuBar::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply rebuild the menu bar again.

    // FIXME: This is called every time a menu shortcut is changed. Rebuilding
    // the menu tree is an expensive operation. Try to avoid multiple rebuilds
    // by calling this only after fixing all the shortcuts.

    rebuildMenuTree();
}
