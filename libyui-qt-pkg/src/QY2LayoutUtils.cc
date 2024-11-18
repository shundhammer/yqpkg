/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


/*
  File:	      QY2LayoutUtils.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  These are pure Qt functions - they can be used independently of YaST2.
*/


#include <qapplication.h>
#include <qwidget.h>
#include <QDesktopWidget>
#include "QY2LayoutUtils.h"


QWidget * addVStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) ); // hor/vert

    return spacer;
}


QWidget * addHStretch( QWidget * parent )
{
    QWidget * spacer = new QWidget( parent );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert

    return spacer;
}


QWidget * addVSpacing( QWidget * parent, int height )
{
    QWidget * spacer = new QWidget( parent );
    Q_CHECK_PTR( spacer );
    spacer->setFixedHeight( height );

    return spacer;
}


QWidget * addHSpacing( QWidget * parent, int width )
{
    QWidget * spacer = new QWidget( parent );
    Q_CHECK_PTR( spacer );
    spacer->setFixedWidth( width );

    return spacer;
}


QSize
limitToScreenSize( const QWidget * widget, int width, int height )
{
    return limitToScreenSize( widget, QSize( width, height ) );
}


QSize
limitToScreenSize( const QWidget * widget, const QSize & desiredSize )
{
    QSize availableSize = QApplication::desktop()->availableGeometry( const_cast<QWidget*> (widget) ).size();

    // Subtract WM decorations. There seems to be no reliable way to tell if
    // this is necessary at all (even fvwm2 claims it is a NETWM compliant
    // window manager) or how large the WM decorations are.
    // For the purpose of this function, let's assume we have to subtract the
    // common fvwm2 decoration size. This is simplistic and should be improved.
    availableSize -= QSize( 10, 35 );

    return desiredSize.boundedTo( availableSize );
}

