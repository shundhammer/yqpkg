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

  File:		YPushButton.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YPushButton_h
#define YPushButton_h

#include "YWidget.h"

class YPushButtonPrivate;



/**
 * A push button; may have an icon, and a F-key shortcut.
 **/
class YPushButton : public YWidget
{
protected:
    /**
     * Constructor.
     **/
    YPushButton( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YPushButton();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YPushButton"; }

    /**
     * Get the label (the text on the button).
     **/
    std::string label() const;

    /**
     * Set the label (the text on the button).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Set this button's icon from an icon file in the UI's default icon
     * directory. Clear the icon if the name is empty.
     *
     * This default implementation does nothing.
     * UIs that can handle icons can choose to overwrite this method.
     **/
    virtual void setIcon( const std::string & iconName ) {}

    /**
     * Returns 'true' if this is the dialog's default button, i.e. the one
     * button that gets activated if the user hits the [Return] key anywhere in
     * the dialog.
     **/
    bool isDefaultButton() const;

    /**
     * Make this button the default button.
     *
     * Derived classes should reimplement this, but call this base class
     * function in the overwritten function.
     **/
    virtual void setDefaultButton( bool def = true );

    /**
     * Set a predefined role for this button.
     *
     * This is important when the button is a child of a YButtonBox so the
     * layout can be arranged according to the conventions of the current UI or
     * desktop environment.
     *
     * See YButtonBox.h for more details. YButtonRole is defined in YTypes.h
     *
     * The default is YCustomButton, i.e., no predefined role.
     * setFunctionKey() uses some heuristics to map function keys to buttons:
     *
     *     F10 -> YOkButton
     *     F9  -> YCancelButton
     *     F1  -> YHelpButton
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class function in the overwritten function.
     **/
    virtual void setRole( YButtonRole role );

    /**
     * Return the role of this button.
     **/
    YButtonRole role() const;

    /**
     * Assign a function key to this widget
     * (1 for F1, 2 for F2, etc.; 0 for none)
     *
     * Reimplemented from YWidget to map function keys to button roles.
     *
     * Derived classes may want to overwrite this function, but they should
     * call this base class function in the new function.
     **/
    virtual void setFunctionKey( int fkey_no );


    /**
     * Returns 'true' if this is a "Help" button.
     *
     * When activated, a help button will traverse up its widget hierarchy and
     * search for the topmost widget with a helpText() set and display that
     * help text in a pop-up dialog (with a local event loop).
     *
     * NOTE that this is only done during YDialog::waitForEvent() (i.e. in YCP
     * UI::WaitForEvent(), UI::UserInput(), UI::TimeoutUserInput() ) and not
     * during YDialog::pollEvent() (i.e. YCP UI::PollInput()) since displaying
     * the help text will block the application until the user closes the help
     * text.
     **/
    bool isHelpButton() const;

    /**
     * Make this button a help button.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method in the overloaded function.
     **/
    virtual void setHelpButton( bool helpButton = true );

    /**
     * Returns 'true' if this is a "Release Notes" button.
     *
     * NOTE that this is only done during YDialog::waitForEvent() (i.e. in YCP
     * UI::WaitForEvent(), UI::UserInput(), UI::TimeoutUserInput() ) and not
     * during YDialog::pollEvent() (i.e. YCP UI::PollInput()) since displaying
     * the release notes will block the application until the user closes the
     * text.
     **/
    bool isRelNotesButton() const;

    /**
     * Make this button a release notes button.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method in the overloaded function.
     **/
    virtual void setRelNotesButton( bool relNotesButton = true );

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
     * Activate the button. Can be used in tests to simulate user input.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activate() = 0;

private:

    ImplPtr<YPushButtonPrivate> priv;
};


std::ostream & operator<<( std::ostream & stream, YButtonRole role );


typedef YPushButton YIconButton;


#endif // YPushButton_h
