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

  File:	      YQReplacePoint.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQReplacePoint_h
#define YQReplacePoint_h

#include <qwidget.h>

#include <yui/YReplacePoint.h>

class QWidget;

class YQReplacePoint : public QWidget, public YReplacePoint
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQReplacePoint( YWidget * parent );

    /**
     * Show a newly added child.
     *
     * Reimplemented from YReplacePoint.
     **/
    virtual void showChild();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQReplacePoint_h
