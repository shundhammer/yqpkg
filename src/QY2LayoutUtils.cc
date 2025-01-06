/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QApplication>
#include <QScreen>
#include <QWidget>

#include "QY2LayoutUtils.h"
#include "Logger.h"
#include "Exception.h"



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
    QSize availableSize = widget->screen()->availableGeometry().size();

    // Subtract WM decorations. There seems to be no reliable way to tell if
    // this is necessary at all (even fvwm2 claims it is a NETWM compliant
    // window manager) or how large the WM decorations are.
    // For the purpose of this function, let's assume we have to subtract the
    // common fvwm2 decoration size. This is simplistic and should be improved.
    availableSize -= QSize( 10, 35 );

    return desiredSize.boundedTo( availableSize );
}

