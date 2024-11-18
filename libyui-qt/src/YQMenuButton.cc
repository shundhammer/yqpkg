/*
  Copyright (C) 2000-2012 Novell, Inc
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

  File:	      YQMenuButton.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QPushButton>
#include <QMenu>
#include <QSize>
#include <QTimer>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQMenuButton.h"
#include <yui/YEvent.h>

using std::string;



YQMenuButton::YQMenuButton( YWidget * 		parent,
			    const string &	label )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YMenuButton( parent, label )
    , _selectedItem( 0 )
{
    setWidgetRep( this );
    _qt_button = new QPushButton( fromUTF8( label ), this );
    _qt_button->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( _qt_button->minimumSize()
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );
}


YQMenuButton::~YQMenuButton()
{
    // NOP
}


void
YQMenuButton::setLabel( const string & label )
{
    _qt_button->setText( fromUTF8( label ) );
    YMenuButton::setLabel( label );
}


void
YQMenuButton::rebuildMenuTree()
{
    //
    // Delete any previous menu
    // (in case the menu items got replaced)
    //

    _actionMap.clear();
    _selectedItem = 0;

    if ( _qt_button->menu() )
	delete _qt_button->menu();


    //
    // Create toplevel menu
    //

    QMenu * menu = new QMenu( _qt_button );
    YUI_CHECK_NEW( menu );
    _qt_button->setMenu( menu );
    menu->setProperty( "class", "ymenubutton QMenu" );

    connect( menu,	&pclass(menu)::triggered,
	     this,	&pclass(this)::menuEntryActivated );

    //
    // Recursively add Qt menu items from the YMenuItems
    //

    rebuildMenuTree( menu, itemsBegin(), itemsEnd() );
}


void
YQMenuButton::rebuildMenuTree( QMenu * parentMenu, YItemIterator begin, YItemIterator end )
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

	    if ( ! icon.isNull() )
		subMenu->setIcon( icon );

	    connect( subMenu,	&pclass(subMenu)::triggered,
		     this,	&pclass(this)::menuEntryActivated );

	    rebuildMenuTree( subMenu, item->childrenBegin(), item->childrenEnd() );
	}
	else // Plain menu item (leaf item)
	{
	    // item->index() is guaranteed to be unique within this YMenuButton's items,
	    // so it can easily be used as unique ID in all Q3PopupMenus that belong
	    // to this YQMenuButton.

            QAction * action = parentMenu->addAction( fromUTF8( item->label() ) );
            item->setUiItem( action );
            _actionMap[ action ] = item;

            if ( ! icon.isNull() )
                action->setIcon( icon );
	}
    }
}


void
YQMenuButton::menuEntryActivated( QAction * action )
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
	 * popups open. So be it - use a zero timer to perform the real
	 * returnNow() later.
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
YQMenuButton::returnNow()
{
    if ( _selectedItem )
    {
	YQUI::ui()->sendEvent( new YMenuEvent( _selectedItem ) );
	_selectedItem = 0;
    }
}


void
YQMenuButton::setItemEnabled( YMenuItem * item, bool enabled )
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
YQMenuButton::setEnabled( bool enabled )
{
    _qt_button->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQMenuButton::preferredWidth()
{
    return 2*YQButtonBorder + _qt_button->sizeHint().width();
}


int YQMenuButton::preferredHeight()
{
    return 2*YQButtonBorder + _qt_button->sizeHint().height();
}


void
YQMenuButton::setSize( int newWidth, int newHeight )
{
    _qt_button->resize( newWidth  - 2 * YQButtonBorder,
                        newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


bool
YQMenuButton::setKeyboardFocus()
{
    _qt_button->setFocus();

    return true;
}


void
YQMenuButton::activateItem( YMenuItem * item )
{
    if ( item )
        YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}
