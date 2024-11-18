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

  File:	      YQMultiProgressMeter.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQMultiProgressMeter_h
#define YQMultiProgressMeter_h


#include <yui/YMultiProgressMeter.h>
#include <qwidget.h>
#include <qpainter.h>
//Added by qt3to4:
#include <qevent.h>
#include <qevent.h>


class QPaintEvent;
class QMouseEvent;


class YQMultiProgressMeter : public QWidget, public YMultiProgressMeter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQMultiProgressMeter( YWidget * 		parent,
			  YUIDimension		dim,
			  const vector<float> &	maxValues );
    /**
     * Destructor.
     **/
    virtual ~YQMultiProgressMeter();

    /**
     * Overall thickness (in pixels) of the MultiProgressMeter.
     **/
    int thickness();

    /**
     * Overall length (in pixels) of the MultiProgressMeter.
     **/
    int length();

    /**
     * Returns the margin around the widget contents.
     **/
    int margin() const { return _margin; }

    /**
     * Sets the margin around the widget contents.
     * Does not trigger an update.
     **/
    void setMargin( int value ) { _margin = value; }

    /**
     * Returns the spacing between segments in pixels.
     **/
    int spacing() const { return _spacing; }

    /**
     * Sets the spacing between segments in pixels.
     * Does not trigger an update.
     **/
    void setSpacing( int value ) { _spacing = value; }

    /**
     * Returns the minimal length of a segment in pixels.
     **/
    int segmentMinLength() const { return _segmentMinLength; }

    /**
     * Set the minimal length of a segment in pixels.
     **/
    void setSegmentMinLength( int val ) { _segmentMinLength = val; }

    /**
     * Returns the thickness (base to point) of the small triangles next to the
     * spacing between individual segments. The base length of those small
     * triangles is double this value minus one. A value <1 indicates no
     * such triangles will be drawn.
     **/
    int triThickness() const { return _triThickness; }

    /**
     * Set the thickness (base to point) of the small triangles next to the
     * spacing between individual segments. The base length of those small
     * triangles is double this value minus one.
     *
     * Use -1 to switch off those triangles - in which case triSpacing (see
     * below) will be set to 0.
     *
     * This call does not trigger a screen update.
     **/
    void setTriThickness( int value );

    /**
     * Returns the spacing between the segment indicators and the small
     * triangles next to the spacing between segments.
     **/
    int triSpacing() const { return _triSpacing; }

    /**
     * Sets the spacing between the segment indicators and the small
     * triangles next to the spacing between segments.
     **/
    void setTriSpacing( int value ) { _triSpacing = value; }

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


protected:

    /**
     * Common initialization
     **/
    void init();

    /**
     * Perform a visual update on the screen.
     * Reimplemented from YMultiProgressMeter.
     **/
    virtual void doUpdate();

    /**
     * Paint the widget's contents.
     * Reimplemented from QWidget.
     **/
    virtual void paintEvent ( QPaintEvent * );

    /**
     * Draw segment number 'segment' with pixel length 'length' from pixel
     * coordinate 'offset' on and fill it according to that segment's current
     * value. 'painter' is set up previously to take rotating into account
     * (horizontal / vertical).
     **/
    void drawSegment( int segment,
		      QPainter & painter,
		      int offset,
		      int length,
		      int thickness );

    /**
     * Draw markers between segments (or beside that spacing).
     **/
    void drawMarkers( QPainter & painter, int offset, int thickness );


private:

    int		_margin;
    int 	_spacing;
    int 	_segmentMinLength;
    int 	_triThickness;
    int		_triSpacing;
};


#endif // YQMultiProgressMeter_h
