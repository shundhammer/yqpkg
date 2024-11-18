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

  File:		YSingleChildContainerWidget.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSingleChildContainerWidget_h
#define YSingleChildContainerWidget_h

#include "YWidget.h"


/**
 * Container widget class that manages one child.
 **/
class YSingleChildContainerWidget: public YWidget
{
protected:
    /**
     * Constructor.
     **/
    YSingleChildContainerWidget( YWidget * parent );

public:
    /**
     * Destructor.
     **/
    virtual ~YSingleChildContainerWidget();

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
     * In this case, the size of the single child is set.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Returns 'true' if this widget is stretchable in the specified dimension.
     * In this case, the stretchability of the single child is returned.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool stretchable( YUIDimension dim ) const;
};



#endif // YSingleChildContainerWidget_h
