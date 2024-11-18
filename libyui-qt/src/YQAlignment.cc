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

  File:	      YQAlignment.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <qpixmap.h>
#include <QPainter>
#include "YQAlignment.h"

using std::string;


YQAlignment::YQAlignment( YWidget *	  	parent,
			  YAlignmentType 	horAlign,
			  YAlignmentType 	vertAlign )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YAlignment( parent, horAlign, vertAlign ), _pixmapFileName()
{
    setWidgetRep( this );
}


YQAlignment::YQAlignment( YWidget *	  	yParent,
			  QWidget *		qParent,
			  YAlignmentType 	horAlign,
			  YAlignmentType 	vertAlign )
    : QWidget( qParent )
    , YAlignment( yParent, horAlign, vertAlign ), _pixmapFileName()
{
    setWidgetRep( this );
}


void YQAlignment::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQAlignment::moveChild( YWidget * child, int newX, int newY )
{
    QWidget * qw = (QWidget *) ( child->widgetRep() );
    qw->move( newX, newY );
}


void YQAlignment::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YAlignment::setSize( newWidth, newHeight );
}


void YQAlignment::paintEvent ( QPaintEvent * event )
{
  QPainter painter(this);
  painter.drawPixmap(rect(), QPixmap(_pixmapFileName.c_str()));

  QWidget::paintEvent(event);
}


void YQAlignment::setBackgroundPixmap( const string & pixmapFileName )
{
    _pixmapFileName = pixmapFileName;

    YAlignment::setBackgroundPixmap( _pixmapFileName );	// Prepend path etc.
    _pixmapFileName = YAlignment::backgroundPixmap();

    this->update();
}

