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

  File:	      YQAlignment.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQAlignment_h
#define YQAlignment_h

#include <qwidget.h>

#include <yui/YAlignment.h>

class QWidget;

class YQAlignment : public QWidget, public YAlignment
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQAlignment( YWidget *		parent,
		 YAlignmentType 	horAlign,
		 YAlignmentType 	vertAlign );

    /**
     * Constructor for cases where the YWidget parent's widgetRep() isn't the
     * QWidget parent, for example in compound widgets like YQWizard where it
     * makes sense to pass a child of the YQWizard (the client area) as the
     * QWidget parent.
     *
     * Use this only if you know very well what you are doing.
     **/
    YQAlignment( YWidget *		yParent,
		 QWidget * 		qParent,
		 YAlignmentType 	horAlign,
		 YAlignmentType 	vertAlign );

    /**
     * Move the child widget to a new position.
     *
     * Implemented from YAlignment.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY );

    /**
     * Enable or disable this widget and its child.
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

    /**
     * Set the background pixmap.
     *
     * Reimplemented from YAlignment.
     **/
     virtual void setBackgroundPixmap( const std::string & pixmapFileName );

protected:
     std::string _pixmapFileName;
     virtual void paintEvent ( QPaintEvent * event );

};


#endif // YQAlignment_h
