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

  File:	      YQCustomStatusItemSelector.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QToolButton>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <yui/YEvent.h>
#include "utf8.h"
#include "YQCustomStatusItemSelector.h"
#include "YQUI.h"


YQCustomStatusItemSelector::YQCustomStatusItemSelector( YWidget *                           parent,
                                                        const YItemCustomStatusVector &     customStates )
    : YQItemSelector( parent, customStates )
{
    preloadStatusIcons();
}


YQCustomStatusItemSelector::~YQCustomStatusItemSelector()
{
    // NOP
}


void YQCustomStatusItemSelector::preloadStatusIcons()
{
    _statusIcons.clear();

    for ( int i=0; i < customStatusCount(); ++i )
        _statusIcons << YQUI::ui()->loadIcon( customStatus( i ).iconName() );
}


void YQCustomStatusItemSelector::addItem( YItem * item )
{
    YUI_CHECK_PTR( item );
    YItemSelector::addItem( item );

    YQCustomStatusSelectorItemWidget * itemWidget =
        new YQCustomStatusSelectorItemWidget( this, item );
    YUI_CHECK_NEW( itemWidget );

    itemWidget->createWidgets();
    _itemWidgets[ item ] = itemWidget;

    connect( itemWidget,        &pclass( itemWidget )::clicked,
             this,              &pclass( this       )::itemClicked );

    // Intentionally not trying to deselect all other items if this one is
    // selected because custom status mode guarantees not to have single
    // selection.
}


void YQCustomStatusItemSelector::selectItem( YItem * item, bool selected )
{
    item->setSelected( selected );
    updateCustomStatusIndicator( item );
}


const QIcon &
YQCustomStatusItemSelector::statusIcon( int status ) const
{
    static QIcon nullIcon = QIcon();

    if ( status >= 0 && status < _statusIcons.size() )
        return _statusIcons.at( status );
    else
        return nullIcon;
}


void YQCustomStatusItemSelector::updateCustomStatusIndicator( YItem * item )
{
    YQSelectorItemWidget * itemWidgetBase = _itemWidgets.value( item );

    if ( ! itemWidgetBase )
	YUI_THROW( YUIException( "Can't find selected item" ) );

    YQCustomStatusSelectorItemWidget * itemWidget =
        dynamic_cast<YQCustomStatusSelectorItemWidget *>( itemWidgetBase );

    if ( itemWidget )
        itemWidget->setStatusIcon();
}


void YQCustomStatusItemSelector::itemClicked( YQCustomStatusSelectorItemWidget * itemWidget )
{
    YUI_CHECK_PTR( itemWidget );
    YItem * item = itemWidget->item();

    int oldStatus = item->status();
    int newStatus = cycleCustomStatus( oldStatus );

    yuiDebug() << "User clicked on item \"" << item->label() << "\""
               << "; new status: " << newStatus
               << endl;

    if ( newStatus >= 0 && newStatus != oldStatus )
    {
        item->setStatus( newStatus );
        itemWidget->setStatusIcon();
    }

    // Send the event even if newStatus == -1: This is particularly interesting
    // for the application so it can handle this on the application level.
    //
    // It might not always be useful or practical for the application to
    // predefine a status transition in the custom status table, in particular
    // if the next status to use depends on other information.

    if ( notify() )
        YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}


void YQCustomStatusItemSelector::activateItem( YItem * item )
{
    // send an activation event for this widget
    if ( notify() )
        YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}



//-----------------------------------------------------------------------------




YQCustomStatusSelectorItemWidget::YQCustomStatusSelectorItemWidget( YQItemSelector      * parent,
                                                                    YItem		* item )
    : YQSelectorItemWidget( parent, item )
{

}


YQCustomStatusSelectorItemWidget::~YQCustomStatusSelectorItemWidget()
{
    // NOP
}


QAbstractButton *
YQCustomStatusSelectorItemWidget::createHeadingToggle( const std::string &  label,
                                                       QWidget *            parent )
{
    QToolButton * toggle = new QToolButton( this );
    YUI_CHECK_NEW( toggle );
    toggle->setText( " " + fromUTF8( label ) );
    toggle->setAutoRaise( true );
    toggle->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    toggle->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, // horizontal: let it grow
                                        QSizePolicy::Fixed ) );        // vertical: use sizeHint()
    setStatusIcon( toggle );

    connect( toggle,    &pclass( toggle )::clicked,
             this,      &pclass( this   )::slotClicked );

    return toggle;
}


void YQCustomStatusSelectorItemWidget::setStatusIcon()
{
    setStatusIcon( _headingToggle );
}


void YQCustomStatusSelectorItemWidget::setStatusIcon( QAbstractButton * toggle )
{
    int status = _item->status();

    if ( ! _parent->validCustomStatusIndex( status ) )
    {
        yuiError() << "Invalid status " << status << " for item " << _item << endl;
        status = 0;
    }

    YQCustomStatusItemSelector * parent = dynamic_cast<YQCustomStatusItemSelector *>( _parent );

    if ( parent )
    {
        QIcon icon = parent->statusIcon( status );

        if ( ! icon.isNull() )
            toggle->setIcon( icon );
    }
}


int YQCustomStatusSelectorItemWidget::itemDescriptionIndent() const
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

    return 24;
}


void YQCustomStatusSelectorItemWidget::slotClicked()
{
    emit clicked( this );
}
