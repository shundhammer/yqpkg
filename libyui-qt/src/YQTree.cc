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

  File:	      YQTree.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QString>
#include <QIcon>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "YQUI.h"
#include <yui/YEvent.h>
#include "utf8.h"
#include "YQTree.h"
#include <yui/YTreeItem.h>
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include "YQApplication.h"

#define VERBOSE_TREE_ITEMS	0

using std::string;



YQTree::YQTree( YWidget *	parent,
		const string &	label,
		bool		multiSelectionMode,
		bool		recursiveSelectionMode	)
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTree( parent, label, multiSelectionMode, recursiveSelectionMode )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin	);

    _nextSerialNo = 0;

    _caption	 = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_treeWidget = new QTreeWidget( this );
    YUI_CHECK_NEW( _qt_treeWidget );
    layout->addWidget( _qt_treeWidget );

    // _qt_treeWidget->setHeaderLabel("");
    // _qt_treeWidget->addColumn( "" );
    _qt_treeWidget->header()->hide();
    // _qt_treeWidget->setHeader(0L);
    _qt_treeWidget->setRootIsDecorated ( true );

    _qt_treeWidget->setContextMenuPolicy( Qt::CustomContextMenu );

    _caption->setBuddy ( _qt_treeWidget );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemSelectionChanged,
	     this,		&pclass(this)::slotSelectionChanged );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemClicked,
	     this,		&pclass(this)::slotItemClicked );

//    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemChanged,
//	     this,		&pclass(this)::slotItemChanged );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemChanged,
	     this,		&pclass(this)::slotItemChanged );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemDoubleClicked,
	     this,		&pclass(this)::slotActivated );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemExpanded,
	     this,		&pclass(this)::slotItemExpanded );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::itemCollapsed,
	     this,		&pclass(this)::slotItemCollapsed );

    connect( _qt_treeWidget,	&pclass(_qt_treeWidget)::customContextMenuRequested,
	     this,		&pclass(this)::slotContextMenu );

}


YQTree::~YQTree()
{
    // NOP
}


void YQTree::setLabel( const string & label )
{
    _caption->setText( label );
    YTree::setLabel( label );
}


void YQTree::rebuildTree()
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );
    _qt_treeWidget->clear();

    buildDisplayTree( 0, itemsBegin(), itemsEnd() );
    _qt_treeWidget->resizeColumnToContents( 0 );
}


void YQTree::buildDisplayTree( YQTreeItem * parentItem, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it < end; ++it )
    {
	YTreeItem * orig = dynamic_cast<YTreeItem *> (*it);
	YUI_CHECK_PTR( orig );

	YQTreeItem * clone;

	if ( parentItem )
	    clone = new YQTreeItem( this, parentItem, orig, _nextSerialNo++ );
	else
	    clone = new YQTreeItem( this, _qt_treeWidget, orig, _nextSerialNo++ );

	YUI_CHECK_NEW( clone );

	if (orig->selected())
	{
	    selectItem(clone);
	}

	if ( orig->hasChildren() )
	    buildDisplayTree( clone, orig->childrenBegin(), orig->childrenEnd() );
    }
}


void YQTree::selectItem( YItem * yItem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    // yuiDebug() << "Selecting item \"" << yItem->label() << "\" " << std::boolalpha << selected << endl;
    YTreeItem * treeItem = dynamic_cast<YTreeItem *> (yItem);
    YUI_CHECK_PTR( treeItem );

    YQTreeItem * yqTreeItem = (YQTreeItem *) treeItem->data();
    YUI_CHECK_PTR( yqTreeItem );


    if ( selected )
    {
	selectItem( yqTreeItem );
    }
    else if ( yqTreeItem == _qt_treeWidget->currentItem() )
    {
	deselectAllItems();
    }
}


void YQTree::selectItem( YQTreeItem * item )
{
    if ( item )
    {
	YQSignalBlocker sigBlocker( _qt_treeWidget );

	_qt_treeWidget->setCurrentItem( item );
	item->setSelected( true );

	if ( hasMultiSelection() )
	    item->setCheckState( 0, Qt::Checked );

	if ( item->parent() )
	    openBranch( (YQTreeItem *) item->parent() );

	YTree::selectItem( item->origItem(), true );

	// yuiDebug() << "selected item: \"" << item->origItem()->label() << "\"" << endl;

    }
}


void YQTree::openBranch( YQTreeItem * item )
{
    while ( item )
    {
	item->setOpen( true ); // Takes care of origItem()->setOpen()
	item = (YQTreeItem *) item->parent();
    }
}


void YQTree::slotItemExpanded( QTreeWidgetItem * qItem )
{
    YQTreeItem * item = dynamic_cast<YQTreeItem *> (qItem);

    if ( item )
	item->setOpen( true );

    _qt_treeWidget->resizeColumnToContents( 0 );
}


void YQTree::slotItemCollapsed( QTreeWidgetItem * qItem )
{
    YQTreeItem * item = dynamic_cast<YQTreeItem *> (qItem);

    if ( item )
	item->setOpen( false );

    _qt_treeWidget->resizeColumnToContents( 0 );
}


void YQTree::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    YTree::deselectAllItems();
    _qt_treeWidget->clearSelection();

    if ( hasMultiSelection() )
    {
	QTreeWidgetItemIterator it( _qt_treeWidget);
	while (*it)
	{
	    YQTreeItem * treeItem = dynamic_cast<YQTreeItem *> (*it);

	    if ( treeItem )
	    {
		treeItem->setCheckState( 0, Qt::Unchecked );
		treeItem->origItem()->setSelected( false );
	    }
	    ++it;
	}
    }

}


void YQTree::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    _qt_treeWidget->clear();
    YTree::deleteAllItems();
}


void YQTree::selectItem(QTreeWidgetItem * item, bool selected, bool recursive)
{

    YQTreeItem * treeItem = dynamic_cast<YQTreeItem *> (item);

    if ( ! treeItem )
	return;

    YSelectionWidget::selectItem( treeItem->origItem(), selected );

    if ( recursive )
    {
	for (int i=0; i < item->childCount(); ++i)
	{
	    QTreeWidgetItem* child = item->child(i);
	    child->setCheckState(0, ( selected )? Qt::Checked : Qt::Unchecked );
	    YQTree::selectItem( child, selected, recursive );
	}
    }

}


void YQTree::slotItemChanged( QTreeWidgetItem * item )
{

    YQSignalBlocker sigBlocker( _qt_treeWidget );

    if ( hasMultiSelection() )
    {
	if ( recursiveSelection() )
	    YQUI::ui()->busyCursor();

	if ( item->checkState(0) == Qt::Checked )
	    YQTree::selectItem( item, true, recursiveSelection() );
	else
	    YQTree::selectItem( item, false, recursiveSelection() );


	if ( recursiveSelection() )
	    YQUI::ui()->normalCursor();

    }
    else
    {
	QList<QTreeWidgetItem *> items = _qt_treeWidget->selectedItems ();

	if ( ! items.empty() )
	{
	    QTreeWidgetItem *qItem = items.first();
	    selectItem( dynamic_cast<YQTreeItem *> (qItem) );
	}
    }


    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQTree::slotItemClicked( QTreeWidgetItem * item, int column )
{
    _qt_treeWidget->setCurrentItem( item );

    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQTree::slotSelectionChanged( )
{
    QList<QTreeWidgetItem *> items = _qt_treeWidget->selectedItems ();

    if ( ! hasMultiSelection() && ! items.empty() )
    {
	QTreeWidgetItem *qItem = items.first();
	selectItem( dynamic_cast<YQTreeItem *> (qItem) );
    }


    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQTree::slotActivated( QTreeWidgetItem * qItem )
{
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


int YQTree::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ? _caption->sizeHint().width() : 0;
    return std::max( 80, hintWidth );
}


int YQTree::preferredHeight()
{
    int hintHeight = !_caption->isHidden() ? _caption->sizeHint().height() : 0;

    // 80 is an arbitrary value.  Use a MinSize or MinHeight widget to set a
    // size that is useful for the application.

    return 80 + hintHeight;
}


void YQTree::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQTree::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_treeWidget->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQTree::setKeyboardFocus()
{
    _qt_treeWidget->setFocus();

    return true;
}


void YQTree::slotContextMenu ( const QPoint & pos )
{
    if	( ! _qt_treeWidget ||  ! _qt_treeWidget->viewport() )
	return;

    YQUI::yqApp()->setContextMenuPos( _qt_treeWidget->viewport()->mapToGlobal( pos ) );
    if ( notifyContextMenu() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
}


YTreeItem *
YQTree::currentItem()
{

    QTreeWidgetItem * currentQItem = _qt_treeWidget->currentItem();

    if ( currentQItem )
    {
	YQTreeItem * item = dynamic_cast<YQTreeItem *> (currentQItem);

	if ( item )
	    return item->origItem();
    }

    return 0;
}


void YQTree::activate()
{
    // send an activation event for this widget
    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this,YEvent::Activated ) );
}

/*============================================================================*/



YQTreeItem::YQTreeItem( YQTree	*	tree,
			QTreeWidget *	listView,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( listView )
{
    init( tree, orig, serial );

#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating toplevel tree item \"" << orig->label() << "\"" << endl;
#endif

}


YQTreeItem::YQTreeItem( YQTree	*	tree,
			YQTreeItem *	parentItem,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( parentItem )
{
    init( tree, orig, serial );
#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating tree item \"" << orig->label()
	       << "\" as child of \"" << parentItem->origItem()->label() << "\""
	       << endl;

#endif
}


void YQTreeItem::init( YQTree *		tree,
		       YTreeItem *	orig,
		       int		serial )
{
    YUI_CHECK_PTR( tree );
    YUI_CHECK_PTR( orig );

    _tree	= tree;
    _serialNo	= serial;
    _origItem	= orig;

    _origItem->setData( this );

    setText( 0, fromUTF8 ( _origItem->label() ) );
    setOpen( _origItem->isOpen() );

    if ( _origItem->hasIconName() )
    {
	QIcon icon = QIcon( _tree->iconFullPath( _origItem ).c_str() );

	if ( icon.isNull() )
	    icon = YQUI::ui()->loadIcon( _origItem->iconName() );

	if ( !icon.isNull() )
	    setData( 0, Qt::DecorationRole, icon );
    }

    if ( tree->hasMultiSelection() )
	setCheckState(0,Qt::Unchecked);
}


void
YQTreeItem::setOpen( bool open )
{
    QTreeWidgetItem::setExpanded( open );
    _origItem->setOpen( open );
}


QString
YQTreeItem::key( int column, bool ascending ) const
{
    /*
     * Sorting key for QListView internal sorting:
     *
     * Always sort tree items by insertion order. The tree widget cannot
     * maintain a meaningful sorting order of its own: All it could do is sort
     * by names (ASCII sort). Better let the application handle this.
     */

    QString strKey = QString( "%1" ).arg( _serialNo,
					  8,		   // fieldWidth (positive aligns right)
					  10,		   // base
					  QChar( '0' ) );  // fillChar

    return strKey;
}
