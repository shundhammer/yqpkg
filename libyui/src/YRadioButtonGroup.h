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

  File:		YRadioButtonGroup.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YRadioButtonGroup_h
#define YRadioButtonGroup_h

#include "YSingleChildContainerWidget.h"

class YRadioButton;
class YRadioButtonGroupPrivate;

typedef std::list<YRadioButton *> 		YRadioButtonList;
typedef YRadioButtonList::iterator		YRadioButtonListIterator;
typedef YRadioButtonList::const_iterator	YRadioButtonListConstIterator;


/**
 * A group of YRadioButton widgets.
 **/
class YRadioButtonGroup : public YSingleChildContainerWidget
{
protected:
    /**
     * Constructor.
     **/
    YRadioButtonGroup( YWidget * parent );

public:
    /**
     * Destructor.
     **/
    virtual ~YRadioButtonGroup();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YRadioButtonGroup"; }

    /**
     * Find the currently selected button.
     **/
    YRadioButton * currentButton() const;

    /**
     * The same as currentButton() above for convenience.
     **/
    YRadioButton * value() const { return currentButton(); }

    /**
     * Add a RadioButton to this button group. RadioButtons are required to
     * call this in their constructor.
     *
     * Derived classes are free to overload this, but they should call this
     * base class function in the overloaded function.
     **/
    virtual void addRadioButton( YRadioButton * radioButton );

    /**
     * Remove a RadioButton from this button group. RadioButtons are required
     * to call this in their destructor, but only if the button group is not
     * also in the process of being destroyed (otherwise there may be race
     * conditions with child widgets already destroyed):
     *
     *     if ( ! buttonGroup()->beingDestroyed )
     *         buttonGroup()->removeRadioButton( this );
     **/
    virtual void removeRadioButton( YRadioButton * radioButton );

    /**
     * Unchecks all radio buttons except one. This method
     * can be used by a concrete UI (the Qt UI or the NCurses UI)
     * in the implementation of YRadioButton::setValue().
     **/
    void uncheckOtherButtons( YRadioButton * radioButton );

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

protected:

    /**
     * Return an iterator that points to the first RadioButton of this button
     * group.
     *
     * Note that RadioButtons in this group may be direct or indirect children
     * of the group, so don't confuse this with YWidget::widgetsBegin().
     **/
    YRadioButtonListConstIterator radioButtonsBegin() const;

    /**
     * Return an iterator that points behind the last RadioButton of this
     * button group.
     **/
    YRadioButtonListConstIterator radioButtonsEnd() const;

    /**
     * Return the number of RadioButtons in this button group.
     **/
    int radioButtonsCount() const;


private:

    ImplPtr<YRadioButtonGroupPrivate> priv;
};


#endif // YRadioButtonGroup_h
