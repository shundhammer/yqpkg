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

  File:		YMultiProgressMeter.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMultiProgressMeter_h
#define YMultiProgressMeter_h

#include "YWidget.h"
#include <vector>

class YMultiProgressMeterPrivate;


/**
 * MultiProgressMeter: Progress bar with several segments that can indicate
 * progress individually. This is useful to display progress of several
 * activities that might not necessarily all be done in sequence.
 *
 * A common example is installing packages from several CDs: Each CD would get
 * a separate segment. Each segment's size would be proportional to the amount
 * of data to be installed from that CD. This visualizes at the same time
 * (a) how many CDs are involved
 * (b) how much in proportion is to be expected from each CD
 * (c) whether or not a specific CD is finished.
 *
 * Visual example (horizontal MultiProgressMeter):
 *
 *     [=============...] [===] [......] [.]
 *
 * This corresponds to 4 CDs:
 *
 * CD #1: A lot of packages are to be installed from this CD, and a fair amount
 *        of those are already installed, but some are still missing.
 * CD #2: Some packages were installed from this, but this CD is finished.
 * CD #3: Quite some packages are to be installed from this CD.
 * CD #4: Very few packages are to be installed from this CD.
 *
 * As can be seen from this simple example, this widget can visualize a lot of
 * complex information at the same time in a very natural way.
 *
 *
 * This is an optional widget, i.e. not all UIs support it.
 **/
class YMultiProgressMeter : public YWidget
{
protected:
    /**
     * Constructor
     **/
    YMultiProgressMeter( YWidget *			parent,
			 YUIDimension			dim,
			 const std::vector<float> &	maxValues );

public:
    /**
     * Destructor.
     **/
    virtual ~YMultiProgressMeter();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMultiProgressMeter"; }

    /**
     * Return the orientation of the MultiProgressBar.
     **/
    YUIDimension dimension() const;

    /**
     * Return 'true' if the orientation is horizontal.
     **/
    bool horizontal() const;

    /**
     * Return 'true' if the orientation is vertical.
     **/
    bool vertical() const;

    /**
     * Return the number of segments.
     **/
    int segments() const;

    /**
     * Return the maximum value for the specified segment (counting from 0).
     **/
    float maxValue( int segment ) const;

    /**
     * Return the current value for the specified segment (counting from 0).
     * If no value has been set yet, -1 is returned.
     **/
    float currentValue( int segment ) const;

    /**
     * Set the current value for the specified segment.
     * This must be in the range 0..maxValue( segment ).
     *
     * Remember to call doUpdate() after all changed values are set!
     **/
    void setCurrentValue( int segment, float value );

    /**
     * Set all current values and call doUpdate().
     **/
    void setCurrentValues( const std::vector<float> & values );

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

    /**
     * Notification that values have been updated and the widget needs to be
     * redisplayed. Derived classes need to reimplement this.
     **/
    virtual void doUpdate() = 0;


private:

    ImplPtr<YMultiProgressMeterPrivate> priv;

};


#endif // YMultiProgressMeter_h
