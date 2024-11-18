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

  File:	      YQLabel.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQLabel_h
#define YQLabel_h

#include <qlabel.h>
#include <yui/YLabel.h>


class YQLabel : public QLabel, public YLabel
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQLabel( YWidget * 		        parent,
	     const std::string &	text,
	     bool 		        isHeading	= false,
	     bool 		        isOutputField	= false );

    /**
     * Destructor.
     **/
    virtual ~YQLabel();

    /**
     * Set the text the widget displays.
     *
     * Reimplemented from YLabel.
     **/
    virtual void setText( const std::string & newText );

    /**
     * Switch bold font on or off.
     *
     * Reimplemented from YLabel.
     **/
    virtual void setUseBoldFont( bool bold );

    /*
     * Enable or disable automatic word wrapping.
     *
     * Reimplemented from YLabel.
     **/
    virtual void setAutoWrap( bool autoWrap = true );

    /**
     * Set enabled / disabled state.
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

protected:

    int _layoutPass1Width;
};


#endif // YQLabel_h
