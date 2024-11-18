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

  File:		YDumbTab.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YDumbTab_h
#define YDumbTab_h

#include "YSelectionWidget.h"

class YDumbTabPrivate;

/**
 * DumbTab: A very simple tab widget that can display and switch between a
 * number of tabs, but will only deliver the "user clicked on tab " event very
 * much like a PushButton does. Actually exchanging the content of the tab is
 * left to the application.
 *
 * DumbTab accepts a single child widget.
 **/
class YDumbTab : public YSelectionWidget
{
protected:
    /**
     * Constructor.
     **/
    YDumbTab( YWidget * parent );

public:
    /**
     * Destructor.
     **/
    virtual ~YDumbTab();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YDumbTab"; }

    /**
     * Add an item (a tab page).
     *
     * Reimplemented from YSelectionWidget.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function in the new implementation.
     **/
    virtual void addItem( YItem * item );

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     *   - if the expected type and the type mismatch
     *   - if the value is out of range
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
     * This method may throw exceptions, for example
     *   - if there is no property with that name
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property set upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

    /**
     * Returns 'true' if this widget is stretchable in the specified dimension.
     * In this case, the stretchability of the single child is returned.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool stretchable( YUIDimension dim ) const;

    /**
     * Descriptive label for debugging. Derived from this widget's only child
     * (if there is one).
     **/
    virtual std::string debugLabel() const;

    /**
    * Activate selected tab. Can be used in tests to simulate user input.
    *
    * Derived classes are required to implement this.
    **/
    virtual void activate() = 0;

private:

    // Disable unwanted base class methods
    std::string label() const;
    virtual void setLabel( const std::string & newLabel ) {};


    ImplPtr<YDumbTabPrivate> priv;
};


#endif // YDumbTab_h
