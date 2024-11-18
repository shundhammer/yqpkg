/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2019 SUSE LLC
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

  File:		YRichText.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YRichText_h
#define YRichText_h

#include <string>
#include "YWidget.h"
#include "ImplPtr.h"


class YRichTextPrivate;


/**
 * Text formatted with simple HTML-like tags, with "links" generating events.
 **/
class YRichText : public YWidget
{
public:

    /**
     * Constructor.
     *
     * 'plainTextMode' indicates that the text should be treated as plain text,
     * i.e. any HTML-like tags in the text should not be interpreted in any
     * way.
     **/
    YRichText( YWidget *		parent,
	       const std::string &	text,
	       bool			plainTextMode = false );

    /**
     * Destructor.
     **/
    virtual ~YRichText();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YRichText"; }

    /**
     * Change the text content of the RichText widget.
     *
     * Derived classes should overwrite this function, but call this base class
     * function in the new function.
     **/
    virtual void setValue( const std::string & newValue );

    /**
     * Return the text content of the RichText widget.
     **/
    std::string value() const;

    /**
     * Alias for setValue().
     **/
    void setText( const std::string & newText ) { setValue( newText ); }

    /**
     * Alias for value().
     **/
    std::string text() const { return value(); }

    /**
     * Return 'true' if this RichText widget is in "plain text" mode, i.e. does
     * not try to interpret RichText/HTML tags.
     **/
    bool plainTextMode() const;

    /**
     * Set this RichText widget's "plain text" mode on or off.
     *
     * Derived classes may want to reimplement this, but they should call this
     * base class function in the new function.
     **/
    virtual void setPlainTextMode( bool on = true );

    /**
     * Return 'true' if this RichText widget should automatically scroll down
     * when the text content is changed. This is useful for progress displays
     * and log files.
     **/
    bool autoScrollDown() const;

    /**
     * Set this RichText widget's "auto scroll down" mode on or off.
     *
     * Derived classes may want to reimplement this, but they should call this
     * base class function in the new function.
     **/
    virtual void setAutoScrollDown( bool on = true );

    /**
     * Returns 'true' if this widget is "shrinkable", i.e. it should be very
     * small by default.
     **/
    bool shrinkable() const;

    /**
     * Make this widget shrinkable, i.e. very small in layouts.
     *
     * This method is intentionally not virtual because it doesn't have any
     * immediate effect; it is only needed in preferredWidth() /
     * preferredHeight().
     **/
    void setShrinkable( bool shrinkable = true );

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
     * Get the position value of the vertical scrollbar.
     *
     * Might not be available in all frontends.
     */
    virtual std::string vScrollValue() const;

    /**
     * Set the position value of the vertical scrollbar. Only values
     * previously obtained using vScrollValue() and some special
     * values are allowed.
     *
     * The special values are:
     *
     * "minimum": Moves the scrollbar to the start.
     * "maximum": Moves the scrollbar to the end.
     *
     * This might not be available in all frontends.
     */
    virtual void setVScrollValue( const std::string & newValue );

    /**
     * Get the position value of the horizontal scrollbar.
     *
     * Might not be available in all frontends.
     */
    virtual std::string hScrollValue() const;

    /**
     * Set the position value of the horizontal scrollbar. Only values
     * previously obtained using hScrollValue() and some special
     * values are allowed.
     *
     * The special values are:
     *
     * "minimum": Moves the scrollbar to the start.
     * "maximum": Moves the scrollbar to the end.
     *
     * The meaning of start and end can depend on the text direction
     * (LTR or RTL).
     *
     * Might not be available in all frontends.
     */
    virtual void setHScrollValue( const std::string & newValue );

    /**
     * Derived classes should implement this, method is used to trigger event
     * like user has pressed the link in the RichText
     **/
    virtual void activateLink( const std::string & url ) = 0;

protected:

    ImplPtr<YRichTextPrivate> priv;
};


#endif // YRichText_h
