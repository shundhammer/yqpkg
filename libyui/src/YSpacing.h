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

  File:		YSpacing.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSpacing_h
#define YSpacing_h

#include "YWidget.h"
#include "ImplPtr.h"

class YSpacingPrivate;


/**
 * HSpacing, VSpacing, HStretch, VStretch
 **/
class YSpacing: public YWidget
{
public:

    /**
     * Constructor.
     *
     * A Spacing/Stretch widget works only in one dimension ('dim') at the same
     * time. But it can be stretchable and have a size at the same time, in
     * which case the specified size acts very much like a minimal size - but
     * not exactly, since YLayoutBox will reduce Spacings first before other
     * widgets have to be resized below their preferred size.
     *
     * 'layoutUnits' is specified in abstract UI units where a main window
     * (800x600 pixels in the Qt UI) corresponds to a 80x25 window.
     **/
    YSpacing( YWidget *		parent,
	      YUIDimension 	dim,
	      bool 		stretchable	= false,
	      YLayoutSize_t 	layoutUnits	= 0.0 );

    /**
     * Destructor.
     **/
    virtual ~YSpacing();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Return the primary dimension of this Spacing/Stretch,
     * i.e. the dimension in which it uses space or stretches.
     **/
    YUIDimension dimension() const;

    /**
     * Return the size in the primary dimension.
     *
     * This is the device dependent size (pixels or character cells), not the
     * abstract UI layout unit from the constructor.
     **/
    int size() const;

    /**
     * Return the size in the specified dimension.
     *
     * This is the device dependent size (pixels or character cells), not the
     * abstract UI layout unit from the constructor.
     **/
    int size( YUIDimension dim ) const;

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


private:

    ImplPtr<YSpacingPrivate> priv;
};


typedef YSpacing YVSpacing;
typedef YSpacing YHSpacing;
typedef YSpacing YHStretch;
typedef YSpacing YVStretch;


#endif // YSpacing_h
