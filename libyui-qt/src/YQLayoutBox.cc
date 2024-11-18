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

  File:	      YQLayoutBox.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include "YQLayoutBox.h"


YQLayoutBox::YQLayoutBox( YWidget * 	parent,
			  YUIDimension	dimension )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YLayoutBox( parent, dimension )
    , _needToEnsureChildrenVisible( false )
{
    setWidgetRep( this );
}


YQLayoutBox::~YQLayoutBox()
{
    // NOP
}


void YQLayoutBox::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQLayoutBox::setSize( int newWidth, int newHeight )
{
    // yuiDebug() << "Resizing " << this << " to " << newWidth << " x " << newHeight << endl;

    if ( _needToEnsureChildrenVisible )
    {
        // This is only useful if child widgets were added after the dialog was
        // initially created: In that case, Qt expects an explicit
        // widget->show() call.
        //
        // Since we don't want to pollute the generic UI layer with such Qt
        // specific things, we do this here since it's the next best place to
        // handle it.

        ensureChildrenVisible();
    }

    resize( newWidth, newHeight );
    YLayoutBox::setSize( newWidth, newHeight );
    _needToEnsureChildrenVisible = true;
}


void YQLayoutBox::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *)( child->widgetRep() );
    qw->move( newX, newY );
}


void YQLayoutBox::ensureChildrenVisible()
{
    // Iterate over the YWidget children, not over the QWidget children so we
    // don't accidentially show Qt widgets that were created in addition to our
    // explicitly created YWidgets. Otherwise, QPopupMenus (e.g. on menu
    // buttons or on menu bars) might be opened immediately - which is not what
    // we want here.

    for ( YWidgetChildrenManager::ChildrenList::const_iterator it = childrenManager()->begin();
          it != childrenManager()->end();
          ++it )
    {
        QWidget * child = (QWidget *) (*it)->widgetRep();

        if ( child && ! child->isVisible() )
            child->show();
    }
}
