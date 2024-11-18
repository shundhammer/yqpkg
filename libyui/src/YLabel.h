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

  File:		YLabel.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YLabel_h
#define YLabel_h

#include "YWidget.h"
#include <string>
#include "ImplPtr.h"


class YLabelPrivate;

/**
 * Implementation of the Label, Heading and OutputField widgets
 **/
class YLabel : public YWidget
{
public:

    /**
     * Constructor.
     *
     * 'isHeading' indicates if this should be displayed as a Heading widget,
     * i.e. with a bold and/or larger font.
     * This cannot be changed after creating the widget.
     *
     * 'isOutputField' indicates if this should be displayed as an OutputField
     * widget, i.e. similar to an InputField the user can't change.
     * This cannot be changed after creating the widget.
     **/
    YLabel( YWidget * 		parent,
	    const std::string &	text,
	    bool 		isHeading	= false,
	    bool 		isOutputField	= false );

    /**
     * Destructor.
     **/
    virtual ~YLabel();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     *
     * Reimplemented from YWidget.
     **/
    virtual const char * widgetClass() const;

    /**
     * Return the text the widget displays.
     **/
    std::string text() const;

    /**
     * Aliases for text().
     **/
    std::string value() const { return text(); }
    std::string label() const { return text(); }

    /**
     * Set the text the widget displays.
     *
     * Derived classes should overwrite this, but call this base class function
     * in the overwritten function.
     **/
    virtual void setText( const std::string & newText );

    /**
     * Aliases for setText().
     **/
    void setValue( const std::string & newValue ) { setText( newValue ); }
    void setLabel( const std::string & newLabel ) { setText( newLabel ); }

    /**
     * Return 'true' if this is a Heading widget, i.e., it should display its
     * text in a bold and/or larger font.
     *
     * This cannot be changed after creating the widget.
     **/
    bool isHeading() const;

    /**
     * Return 'true' if this is an OutputField widget, i.e., it should display
     * its text similar to an InputField the user can't change.
     *
     * This cannot be changed after creating the widget.
     **/
    bool isOutputField() const;

    /**
     * Return 'true' if a bold font should be used.
     **/
    bool useBoldFont() const;

    /**
     * Switch bold font on or off.
     *
     * Derived classes should overwrite this, but call this base class function
     * in the overwritten function.
     **/
    virtual void setUseBoldFont( bool bold = true );

    /**
     * Return 'true' if automatic word wrapping is enabled.
     **/
    bool autoWrap() const;

    /**
     * Enable or disable automatic word wrapping.
     *
     * This has implications for geometry management: An auto-wrapping label
     * does not have any reasonable preferred size; it needs to be put into a
     * parent widget (like a MinSize) that enforces a reasonable width. The
     * height can be then be calculated from that width.
     *
     * Changing this setting takes only effect after the next layout geometry
     * calculation.
     *
     * Derived classes should overwrite this, but call this base class function
     * in the overwritten function.
     **/
    virtual void setAutoWrap( bool autoWrap = true );

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

    /**
     * Returns a descriptive label of this widget instance for debugging.
     *
     * Reimplemented from YWidget since a YLabel doesn't have a shortcut
     * property.
     **/
    virtual std::string debugLabel() const;

protected:

    /**
     * Convenience method for the parent dialog's layoutPass():
     * Return the number of the current layout pass.
     *   0: No layout going on right now
     *   1: First pass
     *   2: Second pass of a multi-pass layout
     **/
    int layoutPass();

private:

    ImplPtr<YLabelPrivate> priv;
};


typedef	YLabel	YHeading;
typedef	YLabel	YOutputField;


#endif // YLabel_h
