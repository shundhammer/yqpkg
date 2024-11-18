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

  File:		YFrame.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YFrame_h
#define YFrame_h

#include <string>
#include "YSingleChildContainerWidget.h"
#include "ImplPtr.h"

class YFramePrivate;


/**
 * A labeled framed container.
 **/
class YFrame : public YSingleChildContainerWidget
{
protected:
    /**
     * Constructor.
     **/
    YFrame( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YFrame();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YFrame"; }

    /**
     * Change the frame label.
     *
     * Derived classes should overwrite this, but call this base class function
     * in the overwritten function.
     **/
    virtual void setLabel( const std::string & newLabel );

    /**
     * Get the current frame label.
     **/
    std::string label() const;

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


private:

    ImplPtr<YFramePrivate> priv;
};


#endif // YFrame_h
