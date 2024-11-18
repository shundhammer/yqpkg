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

  File:	      YQFrame.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQFrame_h
#define YQFrame_h


#include <yui/YFrame.h>
#include <QGroupBox>

class QWidget;

class YQFrame : public QGroupBox, public YFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQFrame( YWidget * 		        parent,
	     const std::string &	label );

    /**
     * Destructor.
     **/
    virtual ~YQFrame();

    /**
     * Change the frame label.
     **/
    virtual void setLabel( const std::string & newLabel );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQFrame_h
