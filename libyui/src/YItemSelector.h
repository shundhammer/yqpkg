/*
  Copyright (c) [2019] SUSE LLC
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

  File:		YItemSelector.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YItemSelector_h
#define YItemSelector_h

#include "YSelectionWidget.h"
#include "YDescribedItem.h"
#include "YItemCustomStatus.h"


class YItemSelectorPrivate;

/**
 * Scrollable item selector widget with not only a label for each item, but
 * also a (possible multi-line) description and an optional icon.
 *
 * This widget supports both 1-of-n or n-of-m selection, i.e. it can act as a
 * (more verbose and more screen space consuming) replacement for YSelectionBox
 * or YMultiSelectionBox.
 **/
class YItemSelector: public YSelectionWidget
{
public:

    /**
     * Standard constructor.
     **/
    YItemSelector( YWidget *    parent,
                   bool         enforceSingleSelection = true );

    /**
     * Constructor for custom item status values.
     *
     * This makes it possible to set a wider variety of values than just 0 or
     * 1.  The semantics behind the individual status values is purely
     * application defined; the specified customStates description only
     * provides an icon (for graphical UIs) or a text representation (for
     * text-based UIs) and an optional "next" status value that can be used to
     * let the user cycle through different status values. The numeric value of
     * each status is implicitly its index in the vector.
     *
     * Notice that this constructor is the only way to set custom status value
     * descriptions; they cannot be changed anymore after initializing the
     * widget. This is by design so that any derived widgets in concrete UIs do
     * not have to bother with possibly recreating any subwidgets if this
     * should change; this guarantees that it does not change, neither the fact
     * that there are custom status values nor their number or indicator icons
     * or texts.
     *
     * This constructor implicitly sets 'enforceSingleSelection' to 'false'.
     *
     * In this mode, the widget sends YMenuEvents (which include the item that
     * the user changed) if the notify option is set.  For anything beyond the
     * simple status transitions that are defined here in 'customStates', it is
     * highly recommended to set that notify option and to handle those
     * YMenuEvents on the application level.
     **/
    YItemSelector( YWidget *                            parent,
                   const YItemCustomStatusVector &      customStates );

    /**
     * Destructor.
     **/
    virtual ~YItemSelector();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Return the number of visible items (i.e. items that are visible without
     * scrolling). This is used to calculate the preferred height. If the
     * widget gets more or less screen space than desired due to layout
     * constraints, this number is not updated; this is purely the desired
     * value for initializing layout negotiations.
     **/
    int visibleItems() const;

    /**
     * Set the number of visible items. When changing this, make sure to
     * recalculate the layout (YDialog::recalc()) so the change has any effect.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method in the overloaded function.
     **/
    virtual void setVisibleItems( int newVal );

    /**
     * Set the status of an item. Unlike YItem::setStatus(), this informs the
     * widget of the change so it can set the corresponding status icon.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setItemStatus( YItem * item, int status );

    /**
     * Return 'true' if this widget uses custom status values, 'false' if not
     * (i.e. only 0 or 1).
     **/
    bool usingCustomStatus() const;

    /**
     * Return the number of custom status values or 0 if no custom status
     * values are used.
     **/
    int customStatusCount() const;

    /**
     * Return the custom status with the specified index (counting from 0).
     *
     * Notice that this may throw a std::out_of_range exception if the index is
     * invalid.
     **/
    const YItemCustomStatus & customStatus( int index );

    /**
     * Return 'true' if a custom status index is within the valid range,
     * i.e. 0..customStatusCount()-1, 'false' otherwise.
     **/
    bool validCustomStatusIndex( int index ) const;

    /**
     * Cycle through the custom status values according to the custom status
     * table, i.e. return the 'nextStatus' field of table index 'oldStatus'.
     * This may be -1 if no next status was specified there or if 'oldStatus'
     * is out of range of that table.
     **/
    int cycleCustomStatus( int oldStatus );

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
    const char * userInputProperty() { return YUIProperty_Value; }

    /**
     * Activate selected item. Can be used in tests to simulate user input.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activateItem( YItem * item ) = 0;

protected:

    /**
     * Update the status indicator (status icon or text indicator) if this
     * widget is using custom status values.
     *
     * Derived classes should overwrite this.
     **/
    virtual void updateCustomStatusIndicator( YItem * item ) {}


private:

    /**
     * Common initializations for all constructors.
     **/
    void init();

    /**
     * Perform a sanity check on the custom status value descriptions.
     **/
    void checkCustomStates();


    // Data members

    ImplPtr<YItemSelectorPrivate> priv;

};      // class YItemSelector

#endif  // YItemSelector_h
