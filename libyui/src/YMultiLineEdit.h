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

  File:		YMultiLineEdit.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMultiLineEdit_h
#define YMultiLineEdit_h

#include "YWidget.h"

class YMultiLineEditPrivate;

/**
 * A multi-line plain-text area
 **/
class YMultiLineEdit : public YWidget
{
protected:
    /**
     * Constructor.
     **/
    YMultiLineEdit( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YMultiLineEdit();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMultiLineEdit"; }

    /**
     * Get the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Derived classes are required to implement this.
     **/
    virtual std::string value() = 0;

    /**
     * Set the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Derived classes are required to implement this.
     **/
    virtual void setValue( const std::string & text ) = 0;

    /**
     * Get the label (the caption above the MultiLineEdit).
     **/
    std::string label() const;

    /**
     * Set the label (the caption above the MultiLineEdit).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * The maximum input length, i.e., the maximum number of characters the
     * user can enter. -1 means no limit.
     **/
    int inputMaxLength() const;

    /**
     * Set the maximum input length, i.e., the maximum number of characters the
     * user can enter. -1 means no limit.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setInputMaxLength( int numberOfChars );

    /**
     * Return the number of input lines that are visible by default.
     *
     * This is what the widget would like to get (which will be reflected by
     * preferredHeight() ), not what it currently actually has due to layout
     * constraints.
     **/
    int defaultVisibleLines() const;

    /**
     * Set the number of input lines that are visible by default.
     *
     * This is what the widget would like to get (which will be reflected by
     * preferredHeight() ), not what it currently actually has due to layout
     * constraints.
     *
     * Notice that since a MultiLineEdit is stretchable in both dimensions, it
     * might get more or less screen space, depending on the layout. This value
     * is only meaningful if there are no other layout constraints.
     *
     * Changing this value will not trigger a re-layout.
     *
     * Derived classes can overwrite this function (but should call this base
     * class function in the new function implementation), but it will normally
     * be sufficient to query defaultVisibleLines() in preferredHeight().
     **/
    virtual void setDefaultVisibleLines( int newVisibleLines );

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

    ImplPtr<YMultiLineEditPrivate> priv;
};


#endif // YMultiLineEdit_h
