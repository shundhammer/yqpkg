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

  File:	      YQMultiSelectionBox.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <limits.h>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQMultiSelectionBox.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"

#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2

using std::string;


YQMultiSelectionBox::YQMultiSelectionBox( YWidget *		parent,
					  const string &	label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMultiSelectionBox( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_listView = new QTreeWidget( this );
    YUI_CHECK_NEW( _qt_listView );
    layout->addWidget( _qt_listView );

    _qt_listView->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    _qt_listView->setHeaderLabel("");	// QListView doesn't have one single column by default!
    _qt_listView->setSortingEnabled( false );

    _qt_listView->header()->hide();
    _qt_listView->setRootIsDecorated ( false );
    _caption->setBuddy( _qt_listView );

    // Very small default size if specified

    connect( _qt_listView,	&pclass(_qt_listView)::itemSelectionChanged,
	     this, 	  	&pclass(this)::slotSelected );

    connect( this,		&pclass(this)::valueChanged,
	     this,		&pclass(this)::slotValueChanged );

    connect( _qt_listView,      &pclass(_qt_listView)::itemChanged,
             this,              &pclass(this)::slotItemChanged );
}


YQMultiSelectionBox::~YQMultiSelectionBox()
{
    // NOP
}


void
YQMultiSelectionBox::setLabel( const string & label )
{
    _caption->setText( label );
    YMultiSelectionBox::setLabel( label );
}


void
YQMultiSelectionBox::addItem( YItem * yItem )
{
    YQSignalBlocker sigBlocker( _qt_listView );
    YMultiSelectionBox::addItem( yItem ); // will also check for NULL

    YQMultiSelectionBoxItem * msbItem = new YQMultiSelectionBoxItem( this, _qt_listView, yItem );

    YUI_CHECK_NEW( msbItem );

    // Take care of the item's check box

    msbItem->setCheckState(0, yItem->selected() ? Qt::Checked : Qt::Unchecked );

    // Take care of the QListView's keyboard focus

    if ( ! _qt_listView->currentItem() )
        _qt_listView->setCurrentItem( msbItem );
}


void YQMultiSelectionBox::selectItem( YItem * yItem, bool selected )
{
    YMultiSelectionBox::selectItem( yItem, selected );
    YQMultiSelectionBoxItem * msbItem = findItem( yItem );

    if ( msbItem )
        msbItem->setCheckState( 0, selected ? Qt::Checked : Qt::Unchecked );
}


void
YQMultiSelectionBox::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );
    YMultiSelectionBox::deselectAllItems();

    QTreeWidgetItemIterator it( _qt_listView );

    while ( *it )
    {
        YQMultiSelectionBoxItem * item = dynamic_cast<YQMultiSelectionBoxItem *> (*it);

        if ( item )
            item->setCheckState(0, Qt::Unchecked);

        ++it;
    }
}


void
YQMultiSelectionBox::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YMultiSelectionBox::deleteAllItems();
    _qt_listView->clear();
}


YItem *
YQMultiSelectionBox::currentItem()
{
    // QListView::currentItem() is very similar, but not exactly the same as
    // QListView::selectedItem(), and it is NOT to be confused with an item's
    // "selected" state in a YQMultiSelectionBox (the item's check box):
    //
    // QListView::currentItem() is the item that currently has the keyboard
    // focus. By default, it is displayed with a faint dotted outline.
    //
    // QListView::selectedItem() is the item that is selected in the QListView
    // widget. It is displayed in a very visible way with inverted colors
    // (typically blue backround). If there is a selected item, it is also the
    // current item. if there is no selected item, there might still be a
    // current item, though.
    //
    // The Y(Q)MultiSelectionBox item's "selected" state is completely
    // independent of all this: It only depends on the item's check
    // box. QListView::selectedItem() and QListView::currentItem() are just
    // mechanisms for keyboard navigation to show the user which item's check
    // box will be toggled when he hits the space bar.
    //
    // For the purpose of this function, QListView::currentItem() is the
    // minimum requirement.

    QTreeWidgetItem * currentQItem = _qt_listView->currentItem();

    if ( currentQItem )
    {
	YQMultiSelectionBoxItem * item = dynamic_cast<YQMultiSelectionBoxItem *> (currentQItem);

	if ( item )
	    return item->yItem();
    }

    return 0;
}


void
YQMultiSelectionBox::setCurrentItem( YItem * yItem )
{
    // See also explanations about QListView::currentItem() vs.
    // QListView::selectedItem() above
    //
    // This function uses QListView::selectedItem() for better visibility.
    // This implicitly also changes QListView::currentItem().

    YQSignalBlocker sigBlocker( _qt_listView );

    if ( ! yItem )
    {
	_qt_listView->clearSelection();
    }
    else
    {
	YQMultiSelectionBoxItem * msbItem = findItem( yItem );

	if ( msbItem )
	    _qt_listView->setCurrentItem( msbItem );

	// This does NOT change the item's check box!
	// (see explanations in YQMultiSelectionBox::currentItem() avove)
    }
}


void
YQMultiSelectionBox::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_listView->setEnabled( enabled );
    //_qt_listView->triggerUpdate();
    YWidget::setEnabled( enabled );
}


int YQMultiSelectionBox::preferredWidth()
{
    int hintWidth = (!_caption->isHidden()) ?
        _caption->sizeHint().width() + frameWidth() : 0;

    return std::max( 80, hintWidth );
}


int YQMultiSelectionBox::preferredHeight()
{
    int hintHeight	 = (!_caption->isHidden()) ? _caption->sizeHint().height() : 0;
    int visibleLines	 = shrinkable() ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
    hintHeight 		+= visibleLines * _qt_listView->fontMetrics().lineSpacing();
    hintHeight		+= _qt_listView->frameWidth() * 2;

    return std::max( 80, hintHeight );
}


void
YQMultiSelectionBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQMultiSelectionBox::setKeyboardFocus()
{
    _qt_listView->setFocus();

    return true;
}


void
YQMultiSelectionBox::slotSelected()
{
    if ( notify() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) ValueChanged event with a SelectionChanged event

	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void
YQMultiSelectionBox::slotValueChanged()
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void
YQMultiSelectionBox::slotItemChanged ( QTreeWidgetItem * _item, int  )
{
    YQMultiSelectionBoxItem * item = dynamic_cast<YQMultiSelectionBoxItem *> (_item);
    bool selected = item->checkState( 0 );
    item->yItem()->setSelected( selected );
    emit valueChanged();
}

YQMultiSelectionBoxItem *
YQMultiSelectionBox::findItem( YItem * wantedItem )
{
    // FIXME: Don't search through all items, use the YItem::data() pointer instead
    QTreeWidgetItemIterator it( _qt_listView );

    while ( *it )
    {
	YQMultiSelectionBoxItem * item = dynamic_cast<YQMultiSelectionBoxItem *> (*it);

	if ( item && item->yItem() == wantedItem )
	    return item;

	++it;
    }

    return 0;
}


int YQMultiSelectionBoxItem::_item_count = 0;



YQMultiSelectionBoxItem::YQMultiSelectionBoxItem( YQMultiSelectionBox *	parent,
						  QTreeWidget * 	listView,
						  YItem *		yItem )
    : QTreeWidgetItem( listView )
    , _yItem( yItem )
    , _multiSelectionBox( parent )
{
    YUI_CHECK_PTR( yItem );
    setFlags( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    setText(0, fromUTF8( yItem->label() ));
    setCheckState( 0, Qt::Unchecked );
    _serial = _item_count++;

    if ( yItem->hasIconName() )
    {
	// _table is checked against 0 in the constructor
	QIcon icon = YQUI::ui()->loadIcon( yItem->iconName() );

	if ( !icon.isNull() )
	    setIcon( 0 /* column */, icon );
    }
}

