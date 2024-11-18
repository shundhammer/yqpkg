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

  File:	      YQDumbTab.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <algorithm>    // std::max

#include <yui/YEvent.h>

#include "utf8.h"
#include "YQLog.h"
#include "YQSignalBlocker.h"
#include "YQUI.h"
#include "YQDumbTab.h"
#include "YQAlignment.h"

#include <QEvent>
#include <QTabBar>


#define YQDumbTabFrameMargin	2

using std::endl;


// NOTICE: This uses Qt widgets in quite unorthodox ways.
// Please read this for an explanation:
//
//   https://github.com/libyui/libyui/pull/31
//
//   https://github.com/libyui/libyui/issues/20


YQDumbTab::YQDumbTab( YWidget *	parent )
    : QTabWidget( (QWidget *) parent->widgetRep() )
    , YDumbTab( parent )
    , _firstPage( 0 )
{
    setWidgetRep( this );

    _tabBar = QTabWidget::tabBar();

    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    _tabBar->setExpanding( false );
    setFocusProxy( _tabBar );
    setFocusPolicy( Qt::TabFocus );

    connect( _tabBar, &pclass( _tabBar )::currentChanged,
	     this,    &pclass( this )::slotSelected );
}


YQDumbTab::~YQDumbTab()
{
    // NOP
}


void YQDumbTab::childEvent( QChildEvent * event )
{
    // Call the parent class method first to ensure all widgets are polished
    QTabWidget::childEvent( event );

    // Reparent all YWidgets:
    //
    // All YQ* widgets call the QWidget parent class constructor with
    // YWidget::parent()->widgetRep() as their QWidget parent, but this is
    // wrong here: This YQDumbTab's widgetRep() is the QTabWidget; but we need
    // to use the _firstPage placeholder instead.
    //
    // So we catch this on the Qt level when the child is added (more exactly:
    // polished) so we can reparent any child widget (typically a
    // YQReplacePoint) to _firstPage.

    if ( event && event->type() == QEvent::ChildPolished )
    {
        // Not using QEvent::ChildAdded here because the child might not yet be
        // completely constructed, so it might not yet be safe to call its
        // YWidget::widgetRep() method; but Qt will send the ChildPolished
        // event for the same one later.

        YWidget * ywidget = dynamic_cast<YWidget *>( event->child() );

        if ( ywidget )
        {
            yuiDebug() << "Reparenting " << ywidget << " to _firstPage" << endl;
            YUI_CHECK_PTR( _firstPage );

            event->child()->setParent( _firstPage );
        }
        else
        {
            yuiDebug() << "Ignoring new " << event->child()->metaObject()->className() << endl;
        }
    }
}


void
YQDumbTab::addItem( YItem * item )
{
    YQSignalBlocker sigBlocker( _tabBar );
    YDumbTab::addItem( item );

    if ( ! _firstPage )
    {
        // Create the first page. There will only be one, even if we have
        // several tabs. See below.

        _firstPage = new QWidget();
        addTab( _firstPage, fromUTF8( item->label() ) );
        yuiDebug() << "Adding first page [" << item->label() << "]" << endl;
    }
    else // There already is a first page
    {
        // We already have a first page; let's not create another one, just add
        // a tab in the tab bar instead without a corresponding page in the
        // QStackedWidget.
        //
        // We don't leave tab switching to the QTabWidget parent class, we just
        // report a click on a tab as a YMenuEvent to the application; it's up
        // to the application to call replaceWidget() with the desired content.
        // So we only want a tab here without a page in the QStackedWidget;
        // that way the QTabWidget can't interfere with page switching.

        _tabBar->insertTab( item->index(), fromUTF8( item->label() ) );
        yuiDebug() << "Adding tab [" << item->label() << "]" << endl;
    }

    if ( item->selected() )
	_tabBar->setCurrentIndex( item->index() );
}


void
YQDumbTab::selectItem( YItem * item, bool selected )
{
    if ( selected )
    {
	// Don't try to suppress any signals sent here with a YQSignalBlocker,
	// otherwise the application code that handles the event will never be executed.

        _tabBar->setCurrentIndex( item->index() );
    }

    YDumbTab::selectItem( item, selected );
}


void
YQDumbTab::deleteAllItems()
{
    if ( _firstPage )
    {
        // Remove the first page from the QStackedWidget.
        // This does not delete the page.
        removeTab( 0 );
    }

    // Remove all remaining tabs

    while ( _tabBar->count() > 0 )
        _tabBar->removeTab( 0 );

    delete _firstPage;
    _firstPage = 0;

    YDumbTab::deleteAllItems();
}


void
YQDumbTab::deselectAllItems()
{
    YDumbTab::deselectAllItems();
}


void
YQDumbTab::slotSelected( int index )
{
    YItem * item = itemAt( index );
    YUI_CHECK_PTR( item );

    yuiDebug() << "Tab [" << item->label() << "] selected" << endl;
    YDumbTab::selectItem( item );

    YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}


void
YQDumbTab::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply set all tab labels again.

    for ( YItemConstIterator it = itemsBegin();
	  it != itemsEnd();
	  ++it )
    {
	YItem * item = *it;
	_tabBar->setTabText( item->index(), fromUTF8( item->label() ) );
    }
}


void
YQDumbTab::setEnabled( bool enabled )
{
    QTabWidget::setEnabled( enabled );
}


int
YQDumbTab::preferredWidth()
{
    int tabBarWidth = _tabBar->sizeHint().width();
    int childWidth  = hasChildren() ? firstChild()->preferredWidth() : 0;

    return std::max( tabBarWidth, childWidth );
}


int
YQDumbTab::preferredHeight()
{
    int tabBarHeight = _tabBar->sizeHint().height();
    int childHeight  = hasChildren() ? firstChild()->preferredHeight() : 0;

    return tabBarHeight + childHeight;
}


void
YQDumbTab::setSize( int newWidth, int newHeight )
{
    // yuiDebug() << "new size: " << QSize( newWidth, newHeight ) << endl;
    QTabWidget::resize( newWidth, newHeight );

    int remainingHeight = newHeight;
    int remainingWidth  = newWidth;

    //
    // _tabBar (fixed height)
    //

    int tabBarHeight = _tabBar->sizeHint().height();

    if ( remainingHeight < tabBarHeight )
	tabBarHeight = remainingHeight;

    remainingHeight -= tabBarHeight;

    if ( hasChildren() )
    {
	//
	// 3D border
	//

	remainingHeight -= 2 * YQDumbTabFrameMargin;
	remainingWidth  -= 2 * YQDumbTabFrameMargin;

	if ( remainingHeight < 0 )
	    remainingHeight = 0;

	if ( remainingWidth < 0 )
	    remainingWidth = 0;

	//
	// Client area
	//

        // yuiDebug() << "client: " << QSize( remainingWidth, remainingHeight ) << "\n" << endl;
	firstChild()->setSize( remainingWidth, remainingHeight );
    }
}


void
YQDumbTab::activate()
{
    // Send an activation event for this widget

    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}
