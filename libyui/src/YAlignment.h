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

  File:		YAlignment.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YAlignment_h
#define YAlignment_h

#include "YSingleChildContainerWidget.h"


class YAlignmentPrivate;

/**
 * Implementation of all the alignment widgets:
 *
 * - Left, Right,  HCenter,
 * - Top,  Bottom, VCenter,
 * - HVCenter
 * - MinSize, MinWidth, MinHeight
 **/
class YAlignment : public YSingleChildContainerWidget
{
protected:
    /**
     * Constructor.
     **/
    YAlignment( YWidget * 	parent,
		YAlignmentType	horAlign,
		YAlignmentType	vertAlign );

public:
    /**
     * Destructor.
     **/
    virtual ~YAlignment();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Return the alignment in the specified dimension.
     **/
    YAlignmentType alignment( YUIDimension dim ) const;

    /**
     * Return the left margin in pixels, the distance between the left edge of
     * this alignment and the left edge of the child widget.
     **/
    int leftMargin() const;

    /**
     * Return the right margin in pixels, the distance between the right edge
     * of this alignment and the right edge of the child widget.
     **/
    int rightMargin() const;

    /**
     * Return the top margin in pixels, the distance between the top edge of
     * this alignment and the top edge of the child widget.
     **/
    int topMargin() const;

    /**
     * Return the bottom margin in pixels, the distance between the bottom
     * edge of this alignment and the bottom edge of the child widget.
     **/
    int bottomMargin() const;

    /**
     * Return the sum of all margins in the specified dimension.
     **/
    int totalMargins( YUIDimension dim ) const;

    /**
     * Set the left margin in pixels.
     **/
    void setLeftMargin( int margin );

    /**
     * Set the right margin in pixels.
     **/
    void setRightMargin( int margin );

    /**
     * Set the top margin in pixels.
     **/
    void setTopMargin( int margin );

    /**
     * Set the bottom margin in pixels.
     **/
    void setBottomMargin( int margin );

    /**
     * Return the minimum width of this alignment or 0 if none is set.
     * preferredWidth() will never return less than this value.
     **/
    int minWidth() const;

    /**
     * Return the minimum height of this alignment or 0 if none is set.
     * preferredHeight() will never return less than this value.
     **/
    int minHeight() const;

    /**
     * Set the minimum width to return for preferredWidth().
     **/
    void setMinWidth( int width );

    /**
     * Set the minimum height to return for preferredHeight().
     **/
    void setMinHeight( int height );

    /**
     * Set a background pixmap.
     *
     * Derived classes may want to overwrite this.
     *
     * This parent method should be called first in the overwritten method to
     * ensure path expansion is done as specified (prepend the theme path
     * ("/usr/share/libyui/theme/") if the path doesn't start with "/"
     * or ".").
     **/
    virtual void setBackgroundPixmap( const std::string & pixmapFileName );

    /**
     * Return the name of the background pixmap or an empty string, if there
     * is none.
     **/
    std::string backgroundPixmap() const;

    /**
     * Add a child widget.
     *
     * Reimplemented from YSingleChildContainerWidget to propagate
     * stretchability down to the single child.
     **/
    virtual void addChild( YWidget * child );

    /**
     * Move a child widget to a new position.
     **/
    virtual void moveChild( YWidget *child, int newx, int newy ) = 0;

    /**
     * Return this widget's stretchability.
     * Reimplemented from YWidget.
     *
     * In an aligned dimension the widget is always stretchable.
     * In an unchanged dimension the widget is stretchable if the
     * child is stretchable.
     **/
    virtual bool stretchable( YUIDimension dim ) const;

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
     * Set the current size and move the child widget according to its
     * alignment.
     *
     * Derived classes should reimplement this, but call this base class
     * function in their own implementation.
     **/
    virtual void setSize( int newWidth, int newHeight );


protected:

    ImplPtr<YAlignmentPrivate> priv;
};


#endif // YAlignment_h
