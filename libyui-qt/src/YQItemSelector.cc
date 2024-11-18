/*
  Copyright (C) 2019 SUSE LLC
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

  File:	      YQItemSelector.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QButtonGroup>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QScrollBar>
#include <QStyle>
#include <QVBoxLayout>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <yui/YEvent.h>
#include "utf8.h"
#include "YQItemSelector.h"
#include "YQSignalBlocker.h"
#include "YQUI.h"

#define ICON_SIZE                       64
#define VERBOSE_SELECTION               0

using std::string;


YQItemSelector::YQItemSelector( YWidget *	parent,
                                bool            enforceSingleSelection )
    : QScrollArea( (QWidget *) parent->widgetRep() )
    , YItemSelector( parent, enforceSingleSelection )
{
    init();
}


YQItemSelector::YQItemSelector( YWidget *                           parent,
                                const YItemCustomStatusVector &     customStates )
    : QScrollArea( (QWidget *) parent->widgetRep() )
    , YItemSelector( parent, customStates )
{
    init();
}


void YQItemSelector::init()
{
    setWidgetRep( this );

    setWidgetResizable( true );
    setSizeAdjustPolicy( QAbstractScrollArea::AdjustToContentsOnFirstShow );

    _itemContainer = new QWidget( this );
    _itemContainer->setObjectName( "itemContainer" );
    YUI_CHECK_NEW( _itemContainer );

    QVBoxLayout * outerVBox = new QVBoxLayout( _itemContainer );
    YUI_CHECK_NEW( outerVBox );

    _itemLayout = new QVBoxLayout();
    outerVBox->addLayout( _itemLayout );
    outerVBox->addStretch( 1000 ); // this takes up any excess space

    _buttonGroup = new QButtonGroup( this );
    YUI_CHECK_NEW( _buttonGroup );

    this->QScrollArea::setWidget( _itemContainer );
}


YQItemSelector::~YQItemSelector()
{
    // NOP
}


void YQItemSelector::addItemWidget( YQSelectorItemWidget * itemWidget )
{
    YUI_CHECK_PTR( itemWidget );

    _itemLayout->addWidget( itemWidget );

    if ( enforceSingleSelection() )
        _buttonGroup->addButton( itemWidget->headingToggle() );
}


void YQItemSelector::addItem( YItem * item )
{
    YUI_CHECK_PTR( item );
    YItemSelector::addItem( item );

    YQSelectorItemWidget * itemWidget = new YQSelectorItemWidget( this, item );
    YUI_CHECK_NEW( itemWidget );

    itemWidget->createWidgets();
    _itemWidgets[ item ] = itemWidget;

    connect( itemWidget,        &pclass( itemWidget )::selectionChanged,
             this,              &pclass( this       )::slotSelectionChanged );

    if ( item->selected() && enforceSingleSelection() )
        deselectOtherItems( item );
}


void YQItemSelector::addItems( const YItemCollection & itemCollection )
{
    for ( YItem * item: itemCollection )
        addItem( item );
}


void YQItemSelector::selectItem( YItem * item, bool selected )
{
    YQSelectorItemWidget * itemWidget = _itemWidgets.value( item );

    if ( ! itemWidget )
	YUI_THROW( YUIException( "Can't find selected item" ) );

    itemWidget->setSelected( selected );

    if ( enforceSingleSelection() )
    {
        if ( selected )
            deselectOtherItems( item );
    }
}


void YQItemSelector::deselectAllItems()
{
    foreach ( YQSelectorItemWidget * itemWidget, _itemWidgets )
        itemWidget->setSelected( false );

    YItemSelector::deselectAllItems();
}


void YQItemSelector::deselectOtherItems( YItem * selectedItem )
{
    for ( QMap<YItem *, YQSelectorItemWidget *>::iterator it = _itemWidgets.begin();
          it != _itemWidgets.end();
          ++it )
    {
        if ( it.key() != selectedItem )
        {
            it.key()->setSelected( false );      // The YItem
            it.value()->setSelected( false );    // ...and the corresponding widget
        }
    }
}


void YQItemSelector::deleteAllItems()
{
    YQSignalBlocker sigBlocker( this );

    qDeleteAll( _itemWidgets.values() );
    _itemWidgets.clear();

    YItemSelector::deleteAllItems();
}


void YQItemSelector::setEnabled( bool enabled )
{
    _itemContainer->setEnabled( enabled );
}


int YQItemSelector::preferredWidth()
{
    int width = _itemContainer->sizeHint().width() + 2;

    QScrollBar * vScrollBar = verticalScrollBar();

    if ( vScrollBar )   // Compensate for vertical scroll bar
        width += vScrollBar->sizeHint().width();

    return width;
}


int YQItemSelector::preferredHeight()
{
    if ( _itemWidgets.size() <= visibleItems() ) // No scrolling necessary
        return _itemContainer->sizeHint().height() + 2;

    // The primitive approach would be to just always use that value. But then
    // all items would always be visible, and the remaining widgets in the
    // layout would have to fight for screen space. Since this widget tends to
    // be a very large one, it would still dominate the layout; cutting off
    // some pixels at its bottom wouldn't affect it much, but any not-so-high
    // widgets like buttons would still be cut off.
    //
    // Thus, we try to add up the needed space for the first n items and return
    // that as the preferred height.

    QList<YQSelectorItemWidget *> visibleItemWidgets =
        findChildren<YQSelectorItemWidget *>().mid( 0, visibleItems() );

    int height = 0;

    // Each item might have a different height, so sum them up individually

    foreach ( YQSelectorItemWidget * itemWidget, visibleItemWidgets )
        height += itemWidget->sizeHint().height();

    if ( ! visibleItemWidgets.isEmpty() )
    {
        height += ( visibleItemWidgets.size() + 0.0 ) * _itemLayout->spacing();
        height += _itemContainer->layout()->contentsMargins().top();
    }

    return height;
}


void YQItemSelector::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQItemSelector::setKeyboardFocus()
{
    YQSelectorItemWidget * itemWidget = findChild<YQSelectorItemWidget *>();

    if ( itemWidget )
    {
        itemWidget->headingToggle()->setFocus();
        return true;
    }
    else
    {
        // yuiMilestone() << "No itemWidget" << endl;
        return false;
    }
}


void YQItemSelector::slotSelectionChanged( YQSelectorItemWidget *   itemWidget,
                                           bool                     selected )
{
    YUI_CHECK_PTR( itemWidget );

    YItem * item = itemWidget->item();
    item->setSelected( selected );

    if ( selected )
    {
#if VERBOSE_SELECTION
        yuiMilestone() << "Selected " << item->label() << endl;
#endif

        if ( enforceSingleSelection() )
            deselectOtherItems( item );
    }
#if VERBOSE_SELECTION
    else
        yuiMilestone() << "Deselected " << item->label() << endl;
#endif

#if VERBOSE_SELECTION
    dumpItems();
#endif

    if ( notify() && ( selected || ! enforceSingleSelection() ) )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQItemSelector::activateItem( YItem * item )
{
    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQItemSelector::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply set all item labels again.

    for ( YItemConstIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	YItem * item = *it;
	_itemWidgets[item]->setLabel( fromUTF8( item->label() ) );
    }
}


//-----------------------------------------------------------------------------


YQSelectorItemWidget::YQSelectorItemWidget( YQItemSelector	* parent,
					    YItem               * item )
    : QFrame( parent->itemContainer() )
    , _parent( parent )
    , _item( item )
{
    // For QSS styling
    setObjectName( "selectorItem" );

    setFirstItemProperty( parent->itemsCount() == 1 );
}


YQSelectorItemWidget::~YQSelectorItemWidget()
{
    // NOP
}


void YQSelectorItemWidget::createWidgets()
{
    string description;
    YDescribedItem * describedItem = dynamic_cast<YDescribedItem *>(_item);

    if ( describedItem )
        description = describedItem->description();

    createWidgets( _item->label(),
                   description,
                   _item->iconName(),
                   _item->selected() );
}


void YQSelectorItemWidget::createWidgets( const string  & label,
                                          const string	& description,
                                          const string	& iconName,
                                          bool		  selected )
{
    /*
     *	   this (QFrame)
     *	     _hBox
     *	       _vBox
     *           _headingToggle
     *		 _descriptionLabel
     *	       _iconLabel
     *
     *	   +--------------------------------------------------+	  QFrame (this)
     *	   |  ( )  Heading			    xx	  xx  |
     *	   |					     xx	 xx   |
     *	   |	   Description text		      Icon    |
     *	   |	   Description text		     xx	 xx   |
     *	   |	   ...				    xx	  xx  |
     *	   |	   Description text			      |
     *	   +--------------------------------------------------+
     */

    _descriptionLabel	= 0;
    _iconLabel		= 0;

    // yuiMilestone() << "Creating item for " << label << endl;


    // Parts initially generated with Qt Designer

    QSizePolicy sizePol( QSizePolicy::Preferred, QSizePolicy::Fixed );
    sizePol.setHorizontalStretch( 0 );
    sizePol.setVerticalStretch( 0 );
    sizePol.setHeightForWidth( sizePolicy().hasHeightForWidth() );
    setSizePolicy( sizePol );

    setFrameShape( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );

    _hBox = new QHBoxLayout( this );	// outer layout
    _hBox->setSpacing( 6 );
    _hBox->setContentsMargins( -1, 6, 6, 6 );

    _vBox = new QVBoxLayout();		// inner layout
    _vBox->setSpacing( 6 );
    _vBox->setContentsMargins( 0, 0, 0, 0 ); // don't let margins accumulate


    //
    // Heading (QRadioButton or QCheckBox)
    //

    _headingToggle = createHeadingToggle( label, this );
    YUI_CHECK_NEW( _headingToggle );

    _headingToggle->setObjectName( "selectorItemHeading" );  // for QSS style sheets
    _headingToggle->setChecked( selected );

    QFont font( _headingToggle->font() );
    font.setBold( true );
    _headingToggle->setFont( font );

    _vBox->addWidget( _headingToggle );
    _hBox->addLayout( _vBox );


    //
    // Description (body text)
    //

    if ( ! description.empty() )
    {
	_descriptionLabel = new QLabel( fromUTF8( description ), this );
	YUI_CHECK_NEW( _descriptionLabel );
	_descriptionLabel->setObjectName( "selectorItemDescription" ); // for QSS
        _descriptionLabel->setIndent( itemDescriptionIndent() ); // Compensate for QRadioButton icon

        _vBox->addWidget( _descriptionLabel );
    }


    //
    // Icon
    //

    if ( ! iconName.empty() )
    {
        _hBox->addStretch( 1000 ); // this takes up any excess space

	_iconLabel = new QLabel( "", this );
        YUI_CHECK_NEW( _iconLabel );

        QIcon icon = YQUI::ui()->loadIcon( iconName );
	_iconLabel->setPixmap( icon.pixmap( ICON_SIZE ) );

	_descriptionLabel->setObjectName( "selectorItemIcon" ); // for QSS
        _iconLabel->setIndent(0);

        QSizePolicy sizePol( _iconLabel->sizePolicy() );
        sizePol.setHorizontalStretch( 0 );
        sizePol.setVerticalStretch( 0 );
        _iconLabel->setSizePolicy( sizePol );

        _hBox->addWidget( _iconLabel );
    }

    YUI_CHECK_PTR( _parent );
    _parent->addItemWidget( this );
}


QAbstractButton *
YQSelectorItemWidget::createHeadingToggle( const std::string &  label,
                                           QWidget *            parent )
{
    QAbstractButton * toggle = 0;

    if ( singleSelection() )
	toggle = new QRadioButton( fromUTF8( label ), this );
    else
	toggle = new QCheckBox( fromUTF8( label ), this );

    YUI_CHECK_NEW( toggle );

    connect( toggle,    &pclass( _headingToggle )::toggled,
             this,      &pclass( this )::slotSelectionChanged );

    return toggle;
}


int YQSelectorItemWidget::itemDescriptionIndent() const
{
    // This magic number in should really come from the widget style and some
    // queries like
    //
    //   style()->pixelMetric( QStyle::PM_RadioButtonLabelSpacing );
    //
    // and then added up from all the necessary individual pieces. But most
    // of those things are never clearly specified. In the Qt code itself
    // there are gems like "width += 4" at strategic places. So there is no
    // realistic way for us on this level to do that right.

    return 20;
}


bool YQSelectorItemWidget::singleSelection() const
{
    return _parent && _parent->enforceSingleSelection();
}


bool YQSelectorItemWidget::selected() const
{
    return _headingToggle->isChecked();
}


void YQSelectorItemWidget::setSelected( bool sel )
{
    YQSignalBlocker sigBlocker( this );
    _headingToggle->setChecked( sel );
}


void YQSelectorItemWidget::slotSelectionChanged( bool selected )
{
    emit selectionChanged( this, selected );
}


void YQSelectorItemWidget::setLabel( const QString & label )
{
    _headingToggle->setText( label );
}


void YQSelectorItemWidget::setFirstItemProperty( bool value )
{
    // Add a dynamic property for use in QSS styling.
    //
    // Use this in QSS as one of the following:
    //
    //   *[first-item="true"] { background-color: orange; }
    //   #selectorItem[first-item="true"] { background-color: orange; }
    //   #selectorItem[first-item="false"] { background-color: cyan; }
    //   #selectorItem { background-color: white; }
    //   
    //   YQItemSelector [first-item="true"] { background-color: orange; }
    //   YQItemSelector [first-item="false"] { background-color: cyan; }
    //   
    //   YQItemSelector #selectorItem { background-color: white; }
    //   YQItemSelector #selectorItem[first-item="true"] { background-color: orange; }
    //   YQItemSelector #selectorItem[first-item="false"] { background-color: cyan; }
    //
    // https://doc.qt.io/qt-5/stylesheet-examples.html#customizing-using-dynamic-properties
    //
    // PR with examples and screenshots:
    // https://github.com/libyui/libyui/pull/68

    setProperty( "first-item", value );
}
