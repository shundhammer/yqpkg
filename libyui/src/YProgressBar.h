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

  File:		YProgressBar.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YProgressBar_h
#define YProgressBar_h

#include "YWidget.h"

class YProgressBarPrivate;


/**
 * A progress bar, showing completion of value() out of maxValue() parts.
 **/
class YProgressBar : public YWidget
{
protected:
    /**
     * Constructor.
     **/
    YProgressBar( YWidget * 		parent,
		  const std::string &	label,
		  int			maxValue = 100 );

public:
    /**
     * Destructor.
     **/
    virtual ~YProgressBar();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YProgressBar"; }

    /**
     * Get the label (the caption above the progress bar).
     **/
    std::string label();

    /**
     * Set the label (the caption above the progress bar).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Return the maximum progress value.
     * Notice that this value can only be set in the constructor.
     **/
    int maxValue() const;

    /**
     * Return the current progress value.
     **/
    int value() const;

    /**
     * Set the current progress value ( <= maxValue() ).
     *
     * Derived classes should reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setValue( int newValue );

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


private:

    ImplPtr<YProgressBarPrivate> priv;
};


#endif // YProgressBar_h
