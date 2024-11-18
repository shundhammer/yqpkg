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

  File:		YSlider.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSlider_h
#define YSlider_h

#include "YIntField.h"

class YSliderPrivate;


/**
 * Slider: Input widget for an integer value between a minimum and a maximum
 * value. Very similar to IntField in semantics, but with a graphical slider
 * that can be dragged to the desired value. It also contains an IntField to
 * allow entering the value directly.
 *
 * Don't confuse this widget with ProgressBar: ProgressBar is output-only.
 *
 *
 * This is an optional widget, i.e. not all UIs support it.
 **/
class YSlider : public YIntField
{
protected:
    /**
     * Constructor.
     *
     * Create a Slider with 'label' as the caption, and the specified minimum
     * and maximum values.
     *
     * Note that YWidgetFactory::createSlider() also has an 'initialValue'
     * parameter that is not used here (because the current value is not stored
     * in this base class, but in the derived class).
     **/
    YSlider( YWidget *			parent,
	     const std::string &	label,
	     int			minValue,
	     int			maxValue );

public:
    /**
     * Destructor.
     **/
    virtual ~YSlider();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YSlider"; }

private:

    ImplPtr<YSliderPrivate> priv;
};


#endif // YSlider_h
