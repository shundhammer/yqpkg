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

  File:		YCheckBoxFrame.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YCheckBoxFrame_h
#define YCheckBoxFrame_h

#include <string>
#include "YSingleChildContainerWidget.h"
#include "ImplPtr.h"

class YCheckBoxFramePrivate;


/**
 * A frame with a check-box, may auto-disable frame contents based on the check.
 * See #setAutoEnable, #invertAutoEnable.
 **/
class YCheckBoxFrame : public YSingleChildContainerWidget
{
public:
    /**
     * Constructor.
     **/
    YCheckBoxFrame( YWidget *		parent,
		    const std::string &	label,
		    bool		checked );

    /**
     * Destructor.
     **/
    virtual ~YCheckBoxFrame();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YCheckBoxFrame"; }

    /**
     * Return the label text on the CheckBoxFrame.
     **/
    std::string label() const;

    /**
     * Change the label text on the CheckBoxFrame.
     *
     * Derived classes should overload this, but call this base class function
     * in the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Check or uncheck the CheckBoxFrame's check box.
     *
     * Derived classes are required to implement this.
     **/
    virtual void setValue( bool isChecked ) = 0;

    /**
     * Get the status of the CheckBoxFrame's check box.
     *
     * Derived classes are required to implement this.
     **/
    virtual bool value() = 0;

    /**
     * Handle children enabling/disabling automatically based on the
     * CheckBoxFrame's check box?
     **/
    bool autoEnable() const;

    /**
     * Change autoEnabled flag.
     *
     * Derived classes are free to overload this, but they should call this
     * base class function in the overloaded function.
     **/
    virtual void setAutoEnable( bool autoEnable );

    /**
     * Invert the meaning of the CheckBoxFrame's check box, i.e., disable child
     * widgets when checked?
     **/
    bool invertAutoEnable() const;

    /**
     * Change invertAutonEnable flag.
     *
     * Derived classes are free to overload this, but they should call this
     * base class function in the overloaded function.
     **/
    virtual void setInvertAutoEnable( bool invertAutoEnable );

    /**
     * Handle enabling/disabling of child widgets based on 'isChecked' (the
     * current status of the check box) and autoEnable() and
     * invertAutoEnable().
     *
     * Derived classes should call this when the check box status changes
     * rather than try to handle it on their level.
     *
     * This method also needs to be called after new child widgets are added to
     * establish the initial enabled or disabled state of the child widgets.
     **/
    void handleChildrenEnablement( bool isChecked );

    /**
     * Get the string of this widget that holds the keyboard shortcut.
     *
     * Reimplemented from YWidget.
     **/
    virtual std::string shortcutString() const { return label(); }

    /**
     * Set the string of this widget that holds the keyboard shortcut.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setShortcutString( const std::string & str )
	{ setLabel( str ); }

    /**
     * The name of the widget property that will return user input.
     * Inherited from YWidget.
     **/
    const char * userInputProperty() { return YUIProperty_Value; }

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

    ImplPtr<YCheckBoxFramePrivate> priv;
};


#endif // YCheckBoxFrame_h
