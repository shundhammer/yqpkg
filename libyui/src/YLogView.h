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

  File:		YLogView.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YLogView_h
#define YLogView_h

#include "YWidget.h"

class YLogViewPrivate;


/**
 * LogView: A scrollable (output-only) text to display a growing log,
 * very much like the "tail -f" shell command.
 **/
class YLogView : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * 'label' is the caption above the log. 'visibleLines' indicates how many
     * lines should be visible by default (unless changed by other layout
     * constraints), 'maxLines' specifies how many lines (always the last ones)
     * to keep in the log. 0 for 'maxLines' means "keep all lines".
     **/
    YLogView( YWidget * 		parent,
	      const std::string &	label,
	      int 			visibleLines,
	      int 			maxLines );

public:

    /**
     * Destructor.
     **/
    virtual ~YLogView();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YLogView"; }

    /**
     * Return the label (the caption above the log text).
     **/
    std::string label() const;

    /**
     * Set the label (the caption above the log text).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Return the number of visible lines.
     **/
    int	visibleLines() const;

    /**
     * Set the number of visible lines. Changing this has only effect upon the
     * next geometry call, so applications calling this function might want to
     * trigger a re-layout afterwards.
     *
     * This method is intentionally not virtual: visibleLines() should be
     * queried in the preferredHeight() implementation.
     **/
    void setVisibleLines( int newVisibleLines );

    /**
     * Return the maximum number of lines to store. The last maxLines() lines
     * of the log text will be kept.
     **/
    int	maxLines() const;

    /**
     * Set the maximum number of lines to store. "0" means "keep all lines"
     * (beware of memory overflow!).
     *
     * If the new value is lower than the old value, any (now) excess lines
     * before the last 'newMaxLines' lines of the log text is cut off and a
     * display update is triggered.
     *
     * This method is intentionally not virtual since a display update is
     * triggered when appropriate.
     **/
    void setMaxLines( int newMaxLines );

    /**
     * Return the entire log text as one large string of concatenated lines
     * delimited with newlines.
     **/
    std::string logText() const;

    /**
     * Set (replace) the entire log text and trigger a display update.
     **/
    void setLogText( const std::string & text );

    /**
     * Return the last log line.
     **/
    std::string lastLine() const;

    /**
     * Append one or more lines to the log text and trigger a display update.
     **/
    void appendLines( const std::string & text );

    /**
     * Clear the log text and trigger a display update.
     **/
    void clearText();

    /**
     * Return the current number of lines.
     **/
    int lines() const;

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


protected:

    /**
     * Display the part of the log text that should be displayed.
     * 'text' contains the last 'visibleLines()' lines.
     * This is called whenever the log text changes. Note that the text might
     * also be empty, in which case the displayed log text should be cleared.
     *
     * Derived classes are required to implement this.
     **/
    virtual void displayLogText( const std::string & text ) = 0;


private:

    /**
     * Append one single line to the log text.
     **/
    void appendLine( const std::string & line );

    /**
     * Trigger a re-display of the log text.
     **/
    void updateDisplay();


    // Data members

    ImplPtr<YLogViewPrivate> priv;

};


#endif // YLogView_h
