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

  File:	      QY2ListView.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include <QPixmap>
#include <QHeaderView>
#include <QMouseEvent>
#include "YQUI.h"
#include "QY2ListView.h"

#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

QY2ListView::QY2ListView( QWidget * parent )
    : QTreeWidget( parent )
    , _mousePressedItem(0)
    , _mousePressedCol( -1 )
    , _mousePressedButton( Qt::NoButton )
    , _sortByInsertionSequence( false )
    , _nextSerial(0)
    , _mouseButton1PressedInHeader( false )
    , _finalSizeChangeExpected( false )
{
    //FIXME QTreeWidget::setShowToolTips( false );
    setRootIsDecorated(false);

#if FIXME_TOOLTIP
    _toolTip = new QY2ListViewToolTip( this );
#endif

    if ( header() )
    {
	header()->installEventFilter( this );
	header()->setStretchLastSection( false );
    }

    connect( header(),	&pclass(header())::sectionResized,
	     this,	&pclass(this)::columnWidthChanged );

    connect( this,      &pclass(this)::itemExpanded,
             this,      &pclass(this)::treeExpanded );

    connect( this,      &pclass(this)::itemCollapsed,
             this,      &pclass(this)::treeCollapsed );

}


QY2ListView::~QY2ListView()
{
#if FIXME_TOOLTIP
    if ( _toolTip )
	delete _toolTip;
#endif
}


void
QY2ListView::selectSomething()
{
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item && (item->flags() & Qt::ItemIsSelectable) )
	{
	    item->setSelected(true); // emits signal, too
	    return;
	}

	++it;
    }
}


void
QY2ListView::clear()
{
    QTreeWidget::clear();
    restoreColumnWidths();
}


void
QY2ListView::updateItemStates()
{
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item )
	    item->updateStatus();

	++it;
    }
}


void
QY2ListView::updateItemData()
{
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item )
	    item->updateData();

	++it;
    }
}


QString
QY2ListView::toolTip( QTreeWidgetItem * listViewItem, int column )
{
    if ( ! listViewItem )
	return QString();

    QString text;

    // text.sprintf( "Column %d:\n%s", column, (const char *) listViewItem->text( column ) );

    // Try known item classes

    QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (listViewItem);

    if ( item )
	return item->toolTip( column );

    QY2CheckListItem * checkListItem = dynamic_cast<QY2CheckListItem *> (listViewItem);

    if ( checkListItem )
	return checkListItem->toolTip( column );

    return QString();
}


void
QY2ListView::saveColumnWidths()
{
    _savedColumnWidth.clear();
    _savedColumnWidth.reserve( columnCount() );

    for ( int i = 0; i < columnCount(); i++ )
    {
	int size = header()->sectionSize(i);
	// yuiMilestone() << "Saving size " << size << " for section " << i << endl;
	_savedColumnWidth.push_back( size );
    }
}


void
QY2ListView::restoreColumnWidths()
{
    if ( _savedColumnWidth.size() != (unsigned) columnCount() ) 	// never manually resized
    {
#if 0
	for ( int i = 0; i < columnCount(); i++ )			// use optimized column width
	    resizeColumnToContents(i);
#endif
    }
    else						// stored settings after manual resizing
    {
	for ( int i = 0; i < columnCount(); i++ )
	{
	    header()->resizeSection( i, _savedColumnWidth[ i ] ); // restore saved column width

#if 0
	    yuiDebug() << "Restoring size " << _savedColumnWidth[i]
		       << " for section " << i
		       << " now " << header()->sectionSize(i)
		       << endl;
#endif
	}
    }
}


void
QY2ListView::mousePressEvent( QMouseEvent * ev )
{
    //y2internal("POS is %d %d", ev->pos().x(), ev->pos().y() );
    QTreeWidgetItem * item = itemAt( ev->pos() );


    if ( item && ( item->flags() & Qt::ItemIsEnabled ) )
    {
	_mousePressedItem	= item;
                _mousePressedCol	= header()->logicalIndexAt( ev->pos().x() );
	_mousePressedButton	= ev->button();
    }
    else	// invalidate last click data
    {
	_mousePressedItem	= 0;
	_mousePressedCol	= -1;
	_mousePressedButton	= Qt::NoButton;
    }

    // Call base class method
    QTreeWidget::mousePressEvent( ev );
}


void
QY2ListView::mouseReleaseEvent( QMouseEvent * ev )
{
    //y2internal("REPOS is %d %d", ev->pos().x(), ev->pos().y() );
    QTreeWidgetItem * item = itemAt( ev->pos() );

    if ( item && ( item->flags() & Qt::ItemIsEnabled ) && item == _mousePressedItem )
    {
 	int col = header()->logicalIndexAt( ev->pos().x() );
        //y2internal("COL %d", col);
 	if ( item == _mousePressedItem	&&
 	     col  == _mousePressedCol	&&
 	     ev->button() == _mousePressedButton )
 	{
 	    emit( columnClicked( ev->button(), item, col, ev->globalPos() ) );
 	}

    }

    // invalidate last click data

    _mousePressedItem	= 0;
    _mousePressedCol	= -1;
    _mousePressedButton	= Qt::NoButton;

    // Call base class method
    QTreeWidget::mouseReleaseEvent( ev );
}


void
QY2ListView::mouseDoubleClickEvent( QMouseEvent * ev )
{
    QTreeWidgetItem * item = itemAt( mapToGlobal( ev->pos() ) );

    if ( item && ( item->flags() & Qt::ItemIsEnabled ) )
    {
 	int col = header()->logicalIndexAt( ev->pos().x() );
 	emit( columnDoubleClicked( ev->button(), (QY2ListViewItem *) item, col, ev->globalPos() ) );
     }

     // invalidate last click data

     _mousePressedItem	= 0;
     _mousePressedCol	= -1;
     _mousePressedButton	= Qt::NoButton;

     // Call base class method
     QTreeWidget::mouseDoubleClickEvent( ev );
}


void
QY2ListView::columnWidthChanged( int, int, int )
{
    saveColumnWidths();

#if 0
    // Workaround for Qt bug:
    //
    // QHeader sends a sizeChange() signal for every size change, not only (as
    // documented) when the user resizes a header column manually. But we only
    // want to record the column widths if the user explicitly did that, so
    // ignore those signals if the mouse isn't pressed. There is also one final
    // sizeChange() signal immediately after the user releases the mouse button.

    if ( _mouseButton1PressedInHeader || _finalSizeChangeExpected )
    {

	// Consume that one sizeChange() signal that is sent immediately after
	// the mouse button is released, but make sure to reset that flag only
	// when appropriate.

	if ( ! _mouseButton1PressedInHeader )
	    _finalSizeChangeExpected = false;
    }
#endif
}


bool
QY2ListView::eventFilter( QObject * obj, QEvent * event )
{
    if ( event && obj && obj == header() )
    {
	if ( event->type() == QEvent::MouseButtonPress )
	{
	    QMouseEvent * mouseEvent = (QMouseEvent *) event;

	    if ( mouseEvent->button() == 1 )
	    {
		_mouseButton1PressedInHeader = true;
		_finalSizeChangeExpected     = false;
	    }
	}
	else if ( event->type() == QEvent::MouseButtonRelease )
	{
	    QMouseEvent * mouseEvent = (QMouseEvent *) event;

	    if ( mouseEvent->button() == 1 )
	    {
		_finalSizeChangeExpected     = true;
		_mouseButton1PressedInHeader = false;
	    }
	}
    }

    return QTreeWidget::eventFilter( obj, event );
}


QSize
QY2ListView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


void
QY2ListView::setSortByInsertionSequence( bool sortByInsertionSequence )
{
    _sortByInsertionSequence = sortByInsertionSequence;
    //FIXME sort();
    header()->setSectionsClickable( ! _sortByInsertionSequence );

}






QY2ListViewItem::QY2ListViewItem( QY2ListView * 	parentListView,
				  const QString &	text )
    : QTreeWidgetItem( parentListView, QStringList(text), 1)
{
    _serial = parentListView->nextSerial();
}


QY2ListViewItem::QY2ListViewItem( QTreeWidgetItem * 	parentItem,
				  const QString &	text )
    : QTreeWidgetItem( parentItem, QStringList(text), 1 )
{
    _serial = 0;

    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( treeWidget() );

    if ( parentListView )
	_serial = parentListView->nextSerial();
}


QY2ListViewItem::~QY2ListViewItem()
{
    // NOP
}


bool
QY2ListViewItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const QY2ListViewItem * other = dynamic_cast<const QY2ListViewItem *> (&otherListViewItem);

    if ( sortByInsertionSequence() )
    {
	if ( other )
	{
            return ( this->serial() < other->serial() );
	}

	// Still here? Try the other version: QY2CheckListItem.

	const QY2CheckListItem * otherCheckListItem = dynamic_cast<const QY2CheckListItem *> (&otherListViewItem);

	if ( otherCheckListItem )
	{
	    return ( this->serial() < otherCheckListItem->serial() );
	}
    }

    int column = treeWidget()->sortColumn();

    if (other)
    {
        return compare(smartSortKey(column), other->smartSortKey(column));
    }

    return compare(text(column).trimmed(), otherListViewItem.text(column).trimmed());
}


bool
QY2ListViewItem::sortByInsertionSequence() const
{
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> (treeWidget());

    if ( parentListView )
	return parentListView->sortByInsertionSequence();

    return false;
}


bool
QY2ListViewItem::compare(const QString& text1, const QString& text2) const
{
    // numeric sorting if columns are numbers

    bool ok1, ok2; // conversion to int successful
    bool retval = text1.toLongLong(&ok1) < text2.toLongLong(&ok2);

    if (ok1 && ok2)
        return retval;     // int < int
    else if (ok1 && !ok2)
        return true;       // int < string
    else if (!ok1 && ok2)
        return false;      // string > int

    // and finally non-numeric sorting is done locale aware
    return QString::localeAwareCompare(text1, text2) < 0;   // string < string
}


QString
QY2ListViewItem::smartSortKey(int column) const
{
    return text(column).trimmed();
}


QY2CheckListItem::QY2CheckListItem( QY2ListView * 		parentListView,
				    const QString &		text )
    : QY2ListViewItem( parentListView, text)
{
    setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setCheckState(0, Qt::Unchecked);
    _serial = parentListView->nextSerial();
}


QY2CheckListItem::QY2CheckListItem( QTreeWidgetItem * 		parentItem,
				    const QString &		text )
    : QY2ListViewItem( parentItem, text)
{
    _serial = 0;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( treeWidget() );

    setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setCheckState(0, Qt::Unchecked);

    if ( parentListView )
	_serial = parentListView->nextSerial();
}

QY2CheckListItem::~QY2CheckListItem()
{
    // NOP
}



#if FIXME_TOOLTIP
void
QY2ListViewToolTip::maybeTip( const QPoint & pos )
{
    Q3Header *       header        = _listView->header();
    QTreeWidgetItem * item          = _listView->itemAt( pos );

    if ( ! item )
	return;

    int x      = _listView->viewportToContents( pos ).x();
    int column = header->sectionAt(x);
    int indent = 0;

    if ( column == 0 )
    {
	indent  =  item->depth() + ( _listView->rootIsDecorated() ? 1 : 0 );
	indent *=  _listView->treeStepSize();

	if ( pos.x() < indent )
	    column = -1;
    }

    QString text = _listView->toolTip( item, column );

    if ( ! text.isEmpty() )
    {
	QRect rect( _listView->itemRect( item ) );

	if ( column < 0 )
	{
	    rect.setX(0);
	    rect.setWidth( indent );
	}
	else
	{
	    QPoint topLeft( header->sectionPos( column ), 0 );
	    topLeft = _listView->contentsToViewport( topLeft );
 	    rect.setX( topLeft.x() );
	    rect.setWidth( header->sectionSize( column ) );
	}

	tip( rect, text );
    }
}

#endif


void QY2ListView::treeExpanded( QTreeWidgetItem * listViewItem )
{
    if ( columnCount() == 1 && header() && header()->isHidden() )
	resizeColumnToContents( 0 );
}


void QY2ListView::treeCollapsed( QTreeWidgetItem * listViewItem )
{
    if ( columnCount() == 1  && header() && header()->isHidden())
	resizeColumnToContents( 0 );
}





