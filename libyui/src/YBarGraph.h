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

  File:		YBarGraph.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YBarGraph_h
#define YBarGraph_h

#include "YWidget.h"
#include "YColor.h"


class YBarGraphPrivate;
class YBarGraphSegment;

/**
 * A graph showing partitioning of a whole.
 * The whole is divided into YBarGraphSegment each of which has
 * a relative size, a text color, a background color, and a label.
 */
class YBarGraph : public YWidget
{
    friend class YBarGraphMultiUpdate;

protected:
    /**
     * Constructor.
     **/
    YBarGraph( YWidget * parent );

public:
    /**
     * Destructor.
     **/
    virtual ~YBarGraph();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YBarGraph"; }

    /**
     * Add one segment.
     *
     * If the segment's background and text colors are not explicitly
     * specified, the YBarGraph widget will assign them from a list of (at
     * least 5 different) color sets.
     *
     * When adding multiple segments, use a YBarGraphMultiUpdate object for
     * improved performance to hold back display updates until all segments are
     * added.
     **/
    void addSegment( const YBarGraphSegment & segment );

    /**
     * Delete all segments.
     **/
    void deleteAllSegments();

    /**
     * Return the current number of segments.
     **/
    int segments() const;

    /**
     * Return the segment with the specified index (from 0 on).
     *
     * This will throw an exception if there are not this many segments.
     **/
    const YBarGraphSegment & segment( int segmentIndex ) const;

    /**
     * Set the value of the segment with the specifie index (from 0 on).
     *
     * This will throw an exception if there are not this many segments.
     *
     * Note: Use a YBarGraphMultiUpdate object for improved performance
     * when doing multiple changes at the same time.
     **/
    void setValue( int segmentIndex, int newValue );

    /**
     * Set the label of the segment with the specified index (from 0 on).
     * Use %1 as a placeholder for the current value.
     *
     * This will throw an exception if there are not this many segments.
     *
     * Note: Use a YBarGraphMultiUpdate object for improved performance
     * when doing multiple changes at the same time.
     **/
    void setLabel( int segmentIndex, const std::string & newLabel );

    /**
     * Set the background color of the segment with the specified index
     * (from 0 on).
     *
     * This will throw an exception if there are not this many segments
     * or if the color is undefined.
     **/
    void setSegmentColor( int segmentIndex, const YColor & color );

    /**
     * Set the text color of the segment with the specified index
     * (from 0 on).
     *
     * This will throw an exception if there are not this many segments
     * or if the color is undefined.
     **/
    void setTextColor( int segmentIndex, const YColor & color );

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This function may throw YUIPropertyExceptions.
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string    & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw YUIPropertyExceptions.
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();


protected:
    /**
     * Perform a display update after any change to any of the segments.
     *
     * Derived classes are required to implement this.
     **/
    virtual void doUpdate() = 0;


private:
    /**
     * Conditionally perform display update if not currently postponed.
     **/
    void updateDisplay();

    ImplPtr<YBarGraphPrivate> priv;
};




/**
 * One segment of a YBarGraph.
 * It has a relative size, a label, label color and background color.
 **/
class YBarGraphSegment
{
public:
    /**
     * Constructor.
     *
     * 'value' is the initial value of this segment.
     *
     * 'label' is the label text in the segment.
     *  Use %1 as a placeholder for the current  value.
     *
     * 'segmentColor' is the background color of this segment.
     *
     * 'textColor' is the color for the label text.
     *
     * The YBarGraph widget will automatically assign some default colors (one
     * of at least 5 different ones) if none are specified.
     **/
    YBarGraphSegment( int 			value 		= 0,
		      const std::string &	label 		= std::string(),
		      const YColor &		segmentColor 	= YColor(),
		      const YColor &		textColor	= YColor() )
	: _value( value )
	, _label( label )
	, _segmentColor( segmentColor )
	, _textColor( textColor )
	{}

    /**
     * Return the current value of this segment.
     **/
    int value() const { return _value; }

    /**
     * Set the value of this segment.
     **/
    void setValue( int newValue ) { _value = newValue; }

    /**
     * Return the current text label of this segment.
     * Any %1 placeholder will be returned as %1 (not expanded).
     **/
    std::string label() const { return _label; }

    /**
     * Set the text label of this segment.
     * Use %1 as a placeholder for the current value.
     **/
    void setLabel( const std::string & newLabel ) { _label = newLabel; }

    /**
     * Return the segment background color.
     **/
    YColor segmentColor() const { return _segmentColor; }

    /**
     * Return 'true' if this segment's background color is defined,
     * i.e. it has a real RGB value and was not just created with the default
     * constructor.
     **/
    bool hasSegmentColor() const { return _segmentColor.isDefined(); }

    /**
     * Set this segment's background color.
     **/
    void setSegmentColor( const YColor & color ) { _segmentColor = color; }

    /**
     * Return this segment's text color.
     **/
    YColor textColor() const { return _textColor; }

    /**
     * Return 'true' if this segment's text color is defined,
     * i.e. it has a real RGB value and was not just created with the default
     * constructor.
     **/
    bool hasTextColor() const { return _textColor.isDefined(); }

    /**
     * Set this segment's text color.
     **/
    void setTextColor( const YColor & color ) { _textColor = color; }


private:

    int		_value;
    std::string	_label;
    YColor 	_segmentColor;
    YColor 	_textColor;
};



/**
 * Helper class for multiple updates to a YBarGraph widget:
 * This will hold back display updates until this object goes out of scope.
 **/
class YBarGraphMultiUpdate
{
public:
    /**
     * Constructor.
     *
     * This will make the corresponding YBarGraph widget hold back any
     * pending display updates (due to changed values, labels, or colors) until
     * this object is destroyed (goes out of scope).
     *
     * Create objects of this class on the stack (as local variables) and
     * simply let them go out of scope.
     *
     * Example:
     *
     * {
     *     YBarGraphMultiUpdate multiUpdate( myBarGraph );
     *     myBarGraph->setValue( 0, 42 ); // No display update yet
     *     myBarGraph->setValue( 1, 84 ); // No display update yet
     *     myBarGraph->setValue( 2, 21 ); // No display update yet
     *
     * } // multiUpdate goes out of scope, will trigger display update now
     *
     **/
    YBarGraphMultiUpdate( YBarGraph * barGraph );

    /**
     * Destructor.
     *
     * This will trigger display updates of the corresponding YBarGraph widget
     * if any are necessary.
     **/
    ~YBarGraphMultiUpdate();

private:

    YBarGraph * _barGraph;
};


#endif // YBarGraph_h
