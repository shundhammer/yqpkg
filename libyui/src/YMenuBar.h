/*
  Copyright (c) [2020] SUSE LLC
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

  File:		YMenuBar.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMenuBar_h
#define YMenuBar_h

#include "YMenuWidget.h"
#include "YMenuItem.h"

class YMenuBarPrivate;


/**
 * A classical menu bar for pulldown menus.
 *
 * Use this only when appropriate: In most places, YaST follows a wizard-driven
 * UI strategy, asking the user (ideally) one question at a time, with a [Next]
 * and a [Back] button to move between wizard steps. One of the last steps is
 * usually presenting the collected information to the user until everything is
 * applied.
 *
 * A menu bar OTOH is meant for the opposite UI strategy: Presenting some kind
 * of document (which may also be a number of input fields) to the user as the
 * central point and providing lots of different operations on that document.
 * The two concepts don't mix very well, so use this widget with caution.
 *
 * A menu bar should only contain menus, no direct actions.
 **/
class YMenuBar: public YMenuWidget
{
protected:
    /**
     * Constructor.
     **/
    YMenuBar( YWidget * parent );

public:

    /**
     * Destructor.
     **/
    virtual ~YMenuBar();

    /**
     * Create a new menu and add it.
     **/
    YMenuItem * addMenu( const std::string & label,
                         const std::string & iconName = "" );

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMenuBar"; }

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

    ImplPtr<YMenuBarPrivate> priv;
};


#endif  // YMenuBar_h
