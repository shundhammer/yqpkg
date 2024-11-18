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

  File:		YSelectionBox.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMultiSelectionBox_h
#define YMultiSelectionBox_h

#include "YSelectionWidget.h"

class YMultiSelectionBoxPrivate;


/**
 * A variant of YSelectionBox where more than one item can be selected.
 **/
class YMultiSelectionBox : public YSelectionWidget
{
protected:
    /**
     * Constructor.
     **/
    YMultiSelectionBox( YWidget * parent, const std::string & label );

public:
    /**
     * Destructor.
     **/
    virtual ~YMultiSelectionBox();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMultiSelectionBox"; }

    /**
     * Return 'true' if this MultiSelectionBox should be very small.
     **/
    bool shrinkable() const;

    /**
     * Make this MultiSelectionBox very small. This will take effect only upon
     * the next geometry management run.
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
     * The name of the widget property that will return user input.
     * Inherited from YWidget.
     **/
    const char * userInputProperty() { return YUIProperty_SelectedItems; }

    /**
     * Return the the item that currently has the keyboard focus
     * or 0 if no item currently has the keyboard focus.
     *
     * Notice that for a MultiSelectionBox the current item is not necessarily
     * selected, i.e., its check box may or may not be checked.
     *
     * Derived classes are required to implement this function.
     **/
    virtual YItem * currentItem() = 0;

    /**
     * Set the keyboard focus to the specified item.
     * 0 means clear the keyboard focus.
     *
     * Notice that for a MultiSelectionBox the current item is not necessarily
     * selected, i.e., its check box may or may not be checked.
     * Use selectItem() for that.
     *
     * Also notice that selectItem() does not make that newly selected item the
     * current item.
     *
     * Derived classes are required to implement this function.
     **/
    virtual void setCurrentItem( YItem * item ) = 0;

    /**
     * Save the widget's user input to a macro recorder.
     *
     * Reimplemented from YWidget because two properties need to be recorded.
     **/
    virtual void saveUserInput( YMacroRecorder *macroRecorder );

private:

    ImplPtr<YMultiSelectionBoxPrivate> priv;
};


#endif // YMultiSelectionBox_h
