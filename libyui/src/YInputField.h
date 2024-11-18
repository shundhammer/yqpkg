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

  File:		YInputField.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YInputField_h
#define YInputField_h

#include <string>
#include "YWidget.h"

class YInputFieldPrivate;



/**
 * InputField: General purpose one line input field for entering text and other
 * data. Can be used for entering passwords with a "*" echoed for every
 * character typed.
 *
 * Like most widgets, the InputField has a label (a caption) above the input
 * field itself. The label can and should get a keyboard shortcut (specified
 * with '&') that will make the input field receive the keyboard focus with a
 * special key combination ("&Name" -> Alt-N or Ctrl-N will make the keyboard
 * focus jump to the corresponding input field).
 **/
class YInputField : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * Create an input field with 'label' as the caption.
     * If 'passwordMode' is set, the input will be not be echoed as clear text.
     **/
    YInputField( YWidget *		parent,
		 const std::string &	label,
		 bool			passwordMode = false );

public:
    /**
     * Destructor.
     **/
    virtual ~YInputField();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Get the current value (the text entered by the user or set from the
     * outside) of this input field.
     *
     * Derived classes are required to implement this.
     **/
    virtual std::string value() = 0;

    /**
     * Set the current value (the text entered by the user or set from the
     * outside) of this input field.
     *
     * Derived classes are required to implement this.
     **/
    virtual void setValue( const std::string & text ) = 0;

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
     * Returns 'true' if this input field is in password mode, i.e. if there
     * should be no on-screen echo or only a '*' for each character typed.
     *
     * Notice that this can only be set in the constructor.
     **/
    bool passwordMode() const;

    /**
     * Get the valid input characters. No input validation is performed (i.e.,
     * the user can enter anything) if this is empty.
     **/
    std::string validChars();

    /**
     * Set the valid input characters. No input validation is performed (i.e.,
     * the user can enter anything) if this is empty.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setValidChars( const std::string & validChars );

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
     * Return 'true' if this InputField should be very small.
     **/
    bool shrinkable() const;

    /**
     * Make this InputField very small. This will take effect only upon the
     * next geometry management run.
     *
     * Derived classes can overwrite this, but should call this base class
     * function in the new function.
     **/
    virtual void setShrinkable( bool shrinkable = true );

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

    /**
     * Save the widget's user input to a macro recorder.
     *
     * Reimplemented from YWidget to avoid recording passwords.
     **/
    virtual void saveUserInput( YMacroRecorder *macroRecorder );

private:

    ImplPtr<YInputFieldPrivate> priv;
};


#endif // YInputField_h
