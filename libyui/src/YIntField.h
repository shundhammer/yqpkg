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

  File:		YIntField.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YIntField_h
#define YIntField_h

#include "YWidget.h"

class YIntFieldPrivate;



/**
 * IntField: Input field for integer values. Enforces input range between a
 * specified minimum and maximum value.
 **/
class YIntField : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * Create an IntField with 'label' as the caption, and the specified minimum
     * and maximum values.
     *
     * Note that YWidgetFactory::createIntField() also has an 'initialValue'
     * parameter that is not used here (because the current value is not stored
     * in this base class, but in the derived class).
     **/
    YIntField( YWidget *		parent,
	       const std::string &	label,
	       int			minValue,
	       int			maxValue );

public:
    /**
     * Destructor.
     **/
    virtual ~YIntField();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YIntField"; }

    /**
     * Get the current value (the number entered by the user or set from the
     * outside) of this IntField.
     *
     * Derived classes are required to implement this.
     **/
    virtual int value() = 0;

    /**
     * Set the current value (the number entered by the user or set from the
     * outside) of this IntField. This method enforces 'val to be between
     * minValue and maxValue.
     **/
    void setValue( int val ) { setValueInternal( enforceRange( val ) ); }

protected:

    /**
     * Set the current value (the number entered by the user or set from the
     * outside) of this IntField. 'val' is guaranteed to be between minValue
     * and maxValue; no further checks are required.
     *
     * Derived classes are required to implement this method.
     **/
    virtual void setValueInternal( int val ) = 0;

    /**
     * Enforce 'val' to be between minValue and maxValue.
     * Return a value that is in range. This does not change the internally
     * stored value of this IntField in any way.
     **/
    int enforceRange( int val ) const;

public:

    /**
     * Return the minimum value.
     **/
    int minValue() const;

    /**
     * Set a new minimum value. If the current value is less than that, it will
     * be set to the new minimum.
     **/
    void setMinValue( int val );

    /**
     * Return the maximum value.
     **/
    int maxValue() const;

    /**
     * Set a new maximum value. If the current value is greater than that, it
     * will be set to the new maximum.
     **/
    void setMaxValue( int val );

    /**
     * Get the label (the caption above the input field).
     **/
    std::string label() const;

    /**
     * Set the label (the caption above the input field).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

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


private:

    ImplPtr<YIntFieldPrivate>	priv;
};


#endif // YIntField_h
