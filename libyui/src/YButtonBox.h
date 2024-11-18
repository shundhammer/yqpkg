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

  File:		YButtonBox.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YButtonBox_h
#define YButtonBox_h

#include <vector>

#include "YWidget.h"
#include "YPushButton.h"

class YButtonBoxPrivate;
class YPushButton;


/**
 * Helper class: Layout policy for YButtonBox widgets.
 * This is used in the default YButtonBox::doLayout() method.
 **/
struct YButtonBoxLayoutPolicy
{
    YButtonBoxLayoutPolicy()
	: buttonOrder( YKDEButtonOrder )
	, equalSizeButtons( false )
	, addExcessSpaceToHelpButtonExtraMargin( false )
    {
	alignment[ YD_HORIZ ] = YAlignCenter;
	alignment[ YD_VERT  ] = YAlignBegin; 	// Align top
    }

    YButtonOrder	buttonOrder;      // YKDEButtonOrder / YGnomeButtonOrder
    bool		equalSizeButtons; // Make all buttons the same size?
    bool		addExcessSpaceToHelpButtonExtraMargin;

    // Alignment for the YButtonBox itself
    YAlignmentType 	alignment[ YUIAllDimensions ];
};


/**
 * Helper class: Margins for YButtonBox widgets.
 * All sizes are in UI-dependent units, i.e. in pixels.
 **/
struct YButtonBoxMargins
{
    YButtonBoxMargins()
	: left( 0 )
	, right( 0 )
	, top( 0 )
	, bottom( 0 )
	, spacing( 0 )
	, helpButtonExtraSpacing( 0 )
	{}

    int	left;			// Left of the leftmost button
    int right;			// Right of the rightmost button
    int top;			// Above the hightest button
    int bottom;			// Below the hightest button

    int spacing;		// Between buttons
    int helpButtonExtraSpacing;	// Between [Help] and the next button
};				// (additionally to "spacing"!)


/**
 * Container widget for dialog buttons that abstracts the button order
 * depending on the desktop environment.
 *
 * KDE and Windows arrange dialog buttons like this:
 *
 *     [OK] [Apply] [Cancel] [Custom1] [Custom2] ... [Help]
 *
 *     [Continue] [Cancel]
 *
 *     [Yes] [No]
 *
 *
 * GNOME and MacOS arrange them like this:
 *
 *     [Help] [Custom1] [Custom2] ... [Apply] [Cancel] [OK]
 *
 *     [Cancel] [Continue]
 *
 *     [No] [Yes]
 *
 *
 * This class provides the abstraction to use whatever layout is more
 * appropriate in the current environment. The application creates the buttons
 * as child widgets of a YButtonBox (rather than a YHBox) and leaves the button
 * order to the YButtonBox.
 *
 * Each of the standard buttons ([OK], [Apply], [Cancel], [Help]) needs to have
 * a button role properly assigned.
 *
 * If set up properly (see YApplication::setDefaultFunctionKey()), known button
 * labels will be assigned an appropriate role:
 *
 *     [OK]			F10
 *     [Continue] -> [OK]	F10
 *     [Yes]      -> [OK]	F10
 *     [Accept]   -> [OK]	F10
 *     [Next]     -> [OK]	F10
 *
 *     [Cancel]			F9
 *     [No]       -> [Cancel]	F9
 *
 *     [Help]			F1
 *
 * Buttons with nonstandard labels that act in such a role need to be
 * explicitly assigned that role:
 *
 *     [Print ] [Cancel] [Help]
 *     [Delete] [Cancel] [Help]
 *
 * Those [Print] or [Delete] buttons act as [OK] buttons (the "yes, do it"
 * action of that dialog). Call YPushButton::setButtonRole( YOkButton )
 * explicitly for them.
 *
 *
 * YButtonBox widgets only accept YPushButton child widgets. Otherwise an
 * exception is thrown.
 *
 * If there is more than one button, one of the child buttons needs to have the
 * [OK] role, and another needs to have the [Cancel] role. Otherwise an
 * exception is thrown.
 **/
class YButtonBox : public YWidget
{
    friend class YButtonBoxPrivate;

protected:
    /**
     * Constructor.
     **/
    YButtonBox( YWidget * parent );

public:

    /**
     * Destructor.
     **/
    virtual ~YButtonBox();

    /**
     * Return a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YButtonBox"; }

    /**
     * Set the button policy for all future YButtonBox widgets:
     * Button order, alignment if there is any excess space, whether or not to
     * give all buttons the same size.
     *
     * You might want to use one of the predefined static methods:
     * YButtonBox::kdeLayoutPolicy(), YButtonBox::gnomeLayoutPolicy().
     *
     * The default doLayout() method uses those values.
     *
     * Notice that there is intentionally no way to set this differently for
     * each individual YButtonBox: This would defeat the purpose of consistency
     * (with the desktop environment this application is running in), which is
     * the reason for having this widget class.
     **/
    static void setLayoutPolicy( const YButtonBoxLayoutPolicy & layoutPolicy );

    /**
     * Return the layout policy.
     **/
    static YButtonBoxLayoutPolicy layoutPolicy();

    /**
     * Predefined layout policy for KDE-like behaviour.
     **/
    static YButtonBoxLayoutPolicy kdeLayoutPolicy();

    /**
     * Predefined layout policy for GNOME-like behaviour.
     **/
    static YButtonBoxLayoutPolicy gnomeLayoutPolicy();

    /**
     * Set the default margins for all future YButtonBox widgets.
     **/
    static void setDefaultMargins( const YButtonBoxMargins & margins );

    /**
     * Return the default margins for all future YButtonBox widgets.
     **/
    static YButtonBoxMargins defaultMargins();

    /**
     * Set the margins for this YButtonBox.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method in the new method.
     **/
    virtual void setMargins( const YButtonBoxMargins & margins );

    /**
     * Return the margins of this YButtonBox.
     *
     * Notice that those are only the desired margins; if there is not enough
     * space, margins and spacings will be reduced before buttons are cut off.
     **/
    YButtonBoxMargins margins() const;

    /**
     * Lay out the button box and its children (its buttons). This is where the
     * button order is implemented.
     *
     * This method is called by the default setSize() method. It uses
     * YButtonBox::layoutPolicy() and the specified margins (defaultMargins
     * unless changed with setMargins() ). It moves the buttons to their
     * position with moveChild().
     *
     * This all should work reasonably in all cases (Qt-UI with KDE button
     * order, Gtk-UI with Gnome button order, NCurses-UI with KDE button
     * order).
     *
     * Still, derived classes can reimplement this. It does not make very much
     * sense to call this default method in a new implementation.
     **/
    virtual void doLayout( int width, int height );

    /**
     * Search the child widgets for the first button with the specified role.
     * Return the button or 0 if there is no button with that role.
     **/
    YPushButton * findButton( YButtonRole role );

    /**
     * Sanity check: Check if all child widgets have the correct widget class
     * and if the button roles are assigned correctly.
     *
     * A YButtonBox with more than one button is required to have one YOKButton
     * and ony YCancelButton. Neither button role may be assigned more than
     * once.
     *
     * This method may throw exceptions:
     * - YUIButtonRoleMismatchException
     * - YUIInvalidChildException	(wrong widget class)
     *
     * This cannot be done as child widgets are inserted since this is done
     * from the child widgets' constructors, so virtual methods or dynamic_cast
     * don't work at that point.
     *
     * This is called in the default setSize() method (just before doLayout()),
     * so any of the above errors are caught only at that time. Applications
     * are free to call this before that time to make error handling more
     * transparent.
     **/
    void sanityCheck();

    /**
     * Relax the sanity check done in sanityCheck(): Do not enforce that there
     * has to be a YOKButton and a YCancelButton if there is more than one
     * button.
     *
     * In very rare cases, it might be neccessary to have a less stringent
     * sanity check: There are some very few legitimate cases for having a
     * YButtonBox with multiple buttons, yet without a YCancelButton.
     *
     * Examples:
     *
     *     ...message...
     *      <Countdown>
     *      [OK] [Stop]
     *
     *     ...message...
     *     [OK] [Details]
     *
     * In those cases, it makes sense to relax the sanity check.
     **/
    void setSanityCheckRelaxed( bool relax = true );

    /**
     * Return 'true' if sanity checks are currently relaxed, 'false' if not.
     **/
    bool sanityCheckRelaxed() const;

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget. This default method returns the sum of
     * the the widths of all child widgets plus the left and right margins plus
     * the spacings.
     *
     * Derived classes can reimplement this method. It does not make very much
     * sense to call this base class method in a new implementation.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget. This default method returns the height of
     * the highest child plus the top and bottom margins.
     *
     * Derived classes can reimplement this method. It does not make very much
     * sense to call this base class method in a new implementation.
     **/
    virtual int preferredHeight();

    /**
     * Sets the size of the YButtonBox.
     *
     * Derived classes can reimplement this, but this base class method should
     * be called in the reimplemented function.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Returns the stretchability of the YButtonBox. YButtonBox widgets are
     * horizontally stretchable by default. How any excess space is used is
     * specified in the layout policy.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool stretchable( YUIDimension dimension ) const;


protected:

    /**
     * Return the button children sorted (left to right) by the current button
     * order (from the layout policy).
     *
     * This default implementation handles KDE and Gnome button orders. It is
     * used in the default doLayout() method.
     *
     * This may throw exceptions if there are non-button children or if there
     * are multiple buttons with any of the standard button roles (except
     * YCustomButton, of course).
     **/
    virtual std::vector<YPushButton *> buttonsByButtonOrder();

    /**
     * Return the (preferred) size of the biggest child widget in the specified
     * dimension.
     **/
    int maxChildSize( YUIDimension dim ) const;

    /**
     * Return the sum of the (preferred) widths of all child widgets.
     **/
    int totalChildrenWidth() const;

    /**
     * Move a child to a new position. This is used in doLayout().
     *
     * Derived classes are required to implement this.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY ) = 0;

    /**
     * Calculate the preferred with with or without trying to enforce buttons
     * of equal size.
     **/
    int preferredWidth( bool equalSizeButtons );


private:

    ImplPtr<YButtonBoxPrivate> priv;

    static YButtonBoxLayoutPolicy	_layoutPolicy;
    static YButtonBoxMargins		_defaultMargins;
};


#endif // YButtonBox_h
