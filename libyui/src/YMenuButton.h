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

  File:		YMenuButton.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YMenuButton_h
#define YMenuButton_h

#include "YMenuWidget.h"
#include "YMenuItem.h"

class YMenuButtonPrivate;


/**
 * MenuButton: Similar to PushButton, but with several actions: Upon clicking
 * on a MenuButton (or activating it with the keyboard), a pop-up menu opens
 * where the user can activate an action. Menu items in that pop-up menu can
 * have submenus (that will pop up in separate pop-up menus).
 *
 * Internally, this widget is more similar to the Tree widget. The difference
 * is that it does not keep a "selected" status, but triggers an action right
 * away, just like a PushButton. Like PushButton, MenuButton sends an event
 * right away when the user selects an item (clicks on a menu item or activates
 * it with the keyboard). Items that have a submenu never send an event, they
 * simply open their submenu when activated.
 **/
class YMenuButton : public YMenuWidget
{
protected:
    /**
     * Constructor.
     *
     * 'label' is the user-visible text on the button (not above it like all
     * other SelectionWidgets).
     **/
    YMenuButton( YWidget * parent, const std::string & label );

public:

    /**
     * Destructor.
     **/
    virtual ~YMenuButton();

    /**
     * Create a new menu item and add it.
     **/
    YMenuItem * addItem( const std::string & label,
                         const std::string & iconName = "" );

    /**
     * Unhide the inherited method shadowed by the above label specialization,
     * YMenuWidget::addItem( YItem * );
     * Their signatures are very different to humans but without the redeclaration
     * the compiler must hide (and warn about) the inherited one.
     * See also https://github.com/libyui/libyui/issues/80
     **/
    using YMenuWidget::addItem;

    /**
     * Create a new submenu and add it.
     **/
    YMenuItem * addMenu( const std::string & label,
                         const std::string & iconName = "" );

    /**
     * Create a new menu separator and add it.
     **/
    YMenuItem * addSeparator();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YMenuButton"; }

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

    ImplPtr<YMenuButtonPrivate> priv;
};


#endif // YMenuButton_h
