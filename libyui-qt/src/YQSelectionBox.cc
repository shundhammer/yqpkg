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

  File:	      YQSelectionBox.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <QString>
#include <QLabel>
#include <QListWidget>
#include <qnamespace.h>
#include <QKeyEvent>
#include <QVBoxLayout>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include <yui/YEvent.h>
#include "YQUI.h"
#include "YQApplication.h"
#include "YQSelectionBox.h"
#include "YQSignalBlocker.h"
#include "YQDialog.h"
#include <yui/YUIException.h>
#include "YQWidgetCaption.h"

#define VERBOSE_SELECTION		1

#define DEFAULT_VISIBLE_LINES		5
#define SHRINKABLE_VISIBLE_LINES	2

using std::string;
using std::endl;


YQSelectionBox::YQSelectionBox( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YSelectionBox( parent, label )
{
    setWidgetRep( this );

    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin	);

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_listWidget = new QListWidget( this );
    YUI_CHECK_NEW( _qt_listWidget );
    layout->addWidget( _qt_listWidget );

    _qt_listWidget->installEventFilter( this );
    //FIXME _qt_listWidget->setVariableHeight( false );
    _qt_listWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
						QSizePolicy::Expanding ) );
    //FIXME _qt_listWidget->setTopItem(0);
    _caption->setBuddy( _qt_listWidget );

    connect( _qt_listWidget,	&pclass(_qt_listWidget)::itemSelectionChanged,
	     this,		&pclass(this)::slotSelectionChanged );

    connect( _qt_listWidget,	&pclass(_qt_listWidget)::itemDoubleClicked,
	     this,		&pclass(this)::slotActivated );

    connect( &_timer,		&pclass(&_timer)::timeout,
	     this,		&pclass(this)::returnImmediately );
}


YQSelectionBox::~YQSelectionBox()
{
    // NOP
}


void YQSelectionBox::setLabel( const string & label )
{
    _caption->setText( label );
    YSelectionBox::setLabel( label );
}


void YQSelectionBox::addItems( const YItemCollection & itemCollection )
{
    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it,
		 true ); // batchMode
    }

    _qt_listWidget->scrollToItem( _qt_listWidget->currentItem(),
				  QAbstractItemView::EnsureVisible );
}


void YQSelectionBox::addItem( YItem * item )
{
    addItem( item,
	     false ); // batchMode
}


void YQSelectionBox::addItem( YItem * item, bool batchMode )
{
    YSelectionBox::addItem( item );
    QIcon icon;

    if ( item->hasIconName() )
    {
	icon = icon = YQUI::ui()->loadIcon( item->iconName() );
    }

    if ( icon.isNull() )
    {
	_qt_listWidget->addItem( fromUTF8( item->label() ) );
    }
    else
    {
	QListWidgetItem *i = new QListWidgetItem( _qt_listWidget );
	i->setData(Qt::DisplayRole, fromUTF8( item->label() ) );
	i->setData(Qt::DecorationRole, icon );
	_qt_listWidget->addItem( i );
    }

    if ( item->selected() )
    {
	YQSignalBlocker sigBlocker( _qt_listWidget );
	_qt_listWidget->setCurrentItem( _qt_listWidget->item( item->index() ) );
    }

    if ( ! batchMode )
    {
	_qt_listWidget->scrollToItem( _qt_listWidget->currentItem(),
				      QAbstractItemView::EnsureVisible );
    }
}


void YQSelectionBox::selectItem( YItem * item, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listWidget );

    YSelectionBox::selectItem( item, selected );
    _qt_listWidget->setCurrentRow( selected ? item->index() : -1 );
}


void YQSelectionBox::selectItem( int index )
{
    YSelectionBox::deselectAllItems();
    YItem * item = YSelectionBox::itemAt( index );

    if ( item )
    {
#ifdef VERBOSE_SELECTION
	yuiDebug() << this << ": Selecting item \"" << item->label() << "\"" << endl;
#endif

	item->setSelected( true );
    }
    else
	YUI_THROW( YUIException( "Can't find selected item" ) );
}


void YQSelectionBox::deselectAllItems()
{
    YSelectionBox::deselectAllItems();
    _qt_listWidget->clearSelection();
    _qt_listWidget->setCurrentRow( -1 );

    if ( _qt_listWidget->currentRow() > -1 )
    {
	// Some item is selected after all; the Qt documtation says this
	// happens if the QListBox is in single selection mode (which it is)
	// and has the keyboard focus. setCurrentRow( -1 ) does the trick for
	// now, but who knows how this might change in future Qt versions.
	//
	// Synchronize internal "selected" flags with what the QListBox
	// displays. This has a small performance penalty because it calls
	// YSelectionBox::deselectAllItems() again which again iterates over
	// all items.

	int index = _qt_listWidget->row( _qt_listWidget->currentItem() );
	selectItem( index );
    }
}


void YQSelectionBox::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listWidget );

    _qt_listWidget->clear();
    YSelectionBox::deleteAllItems();
}



int YQSelectionBox::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ?
	_caption->sizeHint().width() + frameWidth() : 0;

    return std::max( 80, hintWidth );
}


int YQSelectionBox::preferredHeight()
{
    int hintHeight	 = !_caption->isHidden() ? _caption->sizeHint().height() : 0;
    int visibleLines	 = shrinkable() ? SHRINKABLE_VISIBLE_LINES : DEFAULT_VISIBLE_LINES;
    hintHeight		+= visibleLines * _qt_listWidget->fontMetrics().lineSpacing();
    hintHeight		+= _qt_listWidget->frameWidth() * 2;

    return std::max( 80, hintHeight );
}


void YQSelectionBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQSelectionBox::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_listWidget->setEnabled( enabled );
    //FIXME needed? _qt_listWidget->triggerUpdate( true );
    YWidget::setEnabled( enabled );
}


bool YQSelectionBox::setKeyboardFocus()
{
    _qt_listWidget->setFocus();

    return true;
}


bool YQSelectionBox::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
	QKeyEvent * event = ( QKeyEvent * ) ev;

	if ( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
	     ( (event->modifiers() & Qt::NoModifier) || (event->modifiers() & Qt::KeypadModifier) ) )
	{
	    YQDialog * dia = (YQDialog *) findDialog();

	    if ( dia )
	    {
		( void ) dia->activateDefaultButton();
		return true;
	    }
	}
    }
    else if ( ev->type() == QEvent::MouseButtonRelease )
    {
	QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (ev);

	if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
	{
	    yuiMilestone() << "Right click in selecton box detected" << endl;
	    YQUI::yqApp()->maybeLeftHandedUser();
	}
    }
    else if ( ev->type() == QEvent::ContextMenu )
    {
	QContextMenuEvent * contextMenuEvent = dynamic_cast<QContextMenuEvent *> (ev);

	YQUI::yqApp()->setContextMenuPos( contextMenuEvent->globalPos() );
	if ( notifyContextMenu() )
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
    }

    return QWidget::eventFilter( obj, ev );
}


void YQSelectionBox::slotSelectionChanged()
{
    QList<QListWidgetItem *> items = _qt_listWidget->selectedItems();

    if ( ! items.empty() )
    {
	selectItem( _qt_listWidget->row( items.first() ) );
    }
    else
    {
	// Qt thinks it has to outsmart libyui: It might not select anything.
	// So let's get our old selection back. Tit for tat.

	if ( hasItems() && hasSelectedItem() )
	    YQSelectionBox::selectItem( YSelectionWidget::selectedItem(), true );
    }

    if ( notify() )
    {
	if ( immediateMode() )
	    returnImmediately();
	else
	{
	    if ( ! YQUI::ui()->eventsBlocked() )
	    {
		// Delayed event delivery - only if events are to be delivered
		// right now.
		//
		// An event block that is in effect right now may or may not
		// affect events after the timer delay is expired.
		//
		// This may create nasty side effects such as bug #32510: When
		// an item is initially selected, that initial selection event
		// gets through even though (!) events are blocked during
		// widget creation.

		returnDelayed();
	    }
	}
    }
}


void YQSelectionBox::slotActivated( QListWidgetItem * qItem )
{
    selectItem( _qt_listWidget->row( qItem ) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


void YQSelectionBox::returnImmediately()
{
    if ( YQUI::ui()->eventPendingFor( this ) )
    {
	YWidgetEvent * event = dynamic_cast<YWidgetEvent *> ( YQUI::ui()->pendingEvent() );

	if ( event && event->reason() != YEvent::SelectionChanged )
	{
	    // Avoid overwriting a (more important) Activated event with a
	    // SelectionChanged event

	    yuiDebug() << "Not overwriting more important event" << endl;

	    return;
	}
    }


    yuiDebug() << "Sending SelectionChanged event for " << this << endl;
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQSelectionBox::returnDelayed()
{
    yuiDebug() << "Starting selbox timer" << endl;
    _timer.setSingleShot( true );
    _timer.start( 250 ); // millisec
}

