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

  File:	      YQImage.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQImage_h
#define YQImage_h

#include <qlabel.h>
#include <qmovie.h>

#include <yui/YImage.h>
#include <yui/YUI.h>


class YQImage : public QLabel, public YImage
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * 'animated' indicates if 'imageFileName' is an animated image format
     * (e.g., MNG).
     **/
    YQImage( YWidget *		 parent,
	     const std::string & imageFileName,
	     bool		 animated = false );

    /**
     * Destructor.
     **/
    virtual ~YQImage();

    /**
     * Set and display a new image.
     *
     * Reimplemented from YImage.
     **/
    virtual void setImage( const std::string & imageFileName, bool animated = false );

    /**
     * Make the image fit into the available space.
     *
     * Reimplemented from YImage.
     **/
    virtual void setAutoScale( bool autoScale = true );

    /**
      * if false, the image will be displayed in gray
      */
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

    int _pixmapWidth;
    int _pixmapHeight;
};

#endif // YQImage_h
