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

  File:		YShortcut.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YShortcut_h
#define YShortcut_h

#include <iosfwd>
#include <string>
#include <vector>

#include "YWidget.h"
#include "YItem.h"


/**
 * Helper class for shortcut management:
 * This class holds data about the shortcut for one single widget.
 *
 * Notice that there is also a derived class YItemShortcut for item-based
 * widgets that may have multiple shortcuts like YDumbTab, YMenuBar,
 * YItemSelector.
 **/
class YShortcut
{
public:
    /**
     * Constructor
     * @param shortcut_widget (not owned, not nullptr)
     **/
    YShortcut( YWidget *shortcut_widget );

    /**
     * Destructor
     **/
    virtual ~YShortcut();

    /**
     * Marker for "no shortcut"
     **/
    enum { None = 0 };

    /**
     * Returns the YWidget this shortcut data belong to.
     **/
    YWidget * widget() const { return _widget; }

    /**
     * Returns the textual representation of the widget class of the widget
     * this shortcut data belongs to.
     **/
    const char * widgetClass() const { return widget()->widgetClass(); }

    /**
     * Returns 'true' if the widget that is associated with this shortcut is a
     * button (derived from YPushButton).
     **/
    bool isButton() const { return _isButton; }

    /**
     * Returns 'true' if the widget that is associated with this shortcut is a
     * wizard button (one of the navigation buttons of a wizard).
     **/
    bool isWizardButton() const { return _isWizardButton; }

    /**
     * Returns 'true' if this is a shortcut for a (toplevel) menu item.
     *
     * This default implementation always returns 'false'.
     * Derived classes like YItemShortcut can choose to overwrite this.
     **/
    virtual bool isMenuItem() const { return false; }

    /**
     * Returns the complete shortcut string (which may or may not contain "&"),
     * i.e. the value of the widget's shortcut property. For PushButtons, this
     * is the label on the button ( e.g., "&Details..." ), for other widgets
     * usually the caption above it.
     *
     * This value is chached, i.e. this isn't a too expensive operation.
     **/
    std::string shortcutString();

    /**
     * Returns the shortcut string ( from the widget's shortcut property )
     * without any "&" markers.
     *
     * But an escaped "&&" is preserved.
     **/
    std::string cleanShortcutString();

    /**
     * Static version of the above for general use:
     * Returns the specified string without any "&" markers.
     * But an escaped "&&" is preserved.
     **/
    static std::string cleanShortcutString( std::string shortcutString );

    /**
     * The preferred shortcut character, i.e. the character that had been
     * preceded by "&" before checking / resolving conflicts began. 0 if none.
     **/
    char preferred();

    /**
     * The actual shortcut character. 0 if none.
     *
     * This may be different from preferred() if it is overridden.
     **/
    char shortcut();

    /**
     * Set (override) the shortcut character.
     **/
    virtual void setShortcut( char newShortcut );

    /**
     * Clear the shortcut: Override the shortcut character with nothing.
     * This may happen if a conflict cannot be resolved.
     **/
    void clearShortcut();

    /**
     * Query the internal 'conflict' marker. This class doesn't care about that
     * flag, it just stores it for the convenience of higher-level classes.
     **/
    bool conflict() { return _conflict; }

    /**
     * Set or unset the internal 'conflict' marker.
     **/
    void setConflict( bool newConflictState = true ) { _conflict = newConflictState; }

    /**
     * Obtain the number of distinct valid shortcut characters in the shortcut
     * string, i.e. how many different shortcuts that widget could get.
     **/
    int distinctShortcutChars();

    /**
     * Return true if this shortcut contains any character that would be valid
     * as a shortcut character.
     **/
    bool hasValidShortcutChar();

    /**
     * Label of this shortcut suitable for debugging
     **/
    virtual std::string debugLabel() const { return _widget->debugLabel(); }

    /**
     * Static function: Returns the character used for marking keyboard
     * shortcuts.
     **/
    static char shortcutMarker() { return '&'; }

    /**
     * Static function: Find the next occurrence of the shortcut marker ('&')
     * in a string, beginning at starting position start_pos.
     *
     * An escaped "&&" does not count.
     *
     * Returns string::npos if not found or the position of the shortcut marker
     * (not the shortcut character!) if found.
     **/
    static std::string::size_type findShortcutPos( const std::string & str, std::string::size_type start_pos = 0 );

    /**
     * Static function: Find the next shortcut marker in a string, beginning at
     * starting position start_pos.
     *
     * Returns the shortcut character or 0 if none found.
     **/
    static char findShortcut( const std::string & str, std::string::size_type start_pos = 0 );

    /**
     * Returns 'true' if 'c' is a valid shortcut character, i.e. [a-zA-Z0-9],
     * 'false' otherwise.
     **/
    static bool isValid( char c );

    /**
     * Return the normalized version of shortcut character 'c', i.e. a
     * lowercase letter or a digit [a-z0-9]. Returns 0 if 'c' is invalid.
     **/
    static char normalized( char c );

    /**
     * Obtain a widget's shortcut property - the string that contains "&" to
     * designate a shortcut.
     **/
    static std::string getShortcutString( const YWidget * widget );


protected:

    /**
     * Obtain the the shortcut property of this shortcut's widget - the string
     * that contains "&" to designate a shortcut.
     **/
    virtual std::string getShortcutString();


    // Data members

    YWidget *	_widget;               ///< (not owned)
    std::string	_shortcutString;       ///< @see shortcutString
    bool	_shortcutStringCached; ///< is _shortcutString initialized

    std::string	_cleanShortcutString;
    bool	_cleanShortcutStringCached; ///< always false :facepalm:

    /// char or 0 (none found) or -1 (not initialized yet)
    /// @see preferred
    int		_preferred;

    /// char or 0 (none found) or -1 (not initialized yet)
    /// @see shortcut
    int		_shortcut;

    bool	_conflict;       ///< @see conflict
    bool	_isButton;       ///< @see isButton
    bool	_isWizardButton; ///< @see isWizardButton

    /// -1 means uninitialized
    /// @see distinctShortcutChars
    int		_distinctShortcutChars;
};



/**
 * Special case for item-based widgets that can have multiple shortcuts like
 * YDumbTab, YMenuBar, YItemSelector. Each instance represents one of those
 * items.
 *
 * For YMenuBar, only the toplevel items are handled here since only their
 * shortcuts need to be unique within the dialog. Within each menu, shortcut
 * conflicts are resolved separately in YMenuWidget.
 **/
class YItemShortcut: public YShortcut
{
public:
    /**
     * Constructor.
     **/
    YItemShortcut( YWidget * widget, YItem * item );

    /**
     * Destructor.
     **/
    virtual ~YItemShortcut() {}

    /**
     * Return the associated item.
     **/
    YItem * item() const { return _item; }

    /**
     * Set (override) the shortcut character.
     *
     * Reimplemented from YShortcut.
     * In this subclass, it will change the internally stored item.
     **/
    virtual void setShortcut( char newShortcut );

    /**
     * Returns 'true' if this is a shortcut for a (toplevel) menu item.
     *
     * Reimplemented from YShortcut.
     **/
    virtual bool isMenuItem() const { return _isMenuItem; }

    /**
     * Label of this shortcut suitable for debugging
     **/
    virtual std::string debugLabel() const
        { return cleanShortcutString( _item->debugLabel() ); }


protected:

    /**
     * Obtain the the shortcut property of this shortcut's widget - the string
     * that contains "&" to designate a shortcut.
     **/
    virtual std::string getShortcutString();


private:

    YItem * _item;
    bool    _isMenuItem;
};


std::ostream & operator<<( std::ostream & stream, const YShortcut * shortcut );


typedef std::vector<YShortcut *>	YShortcutList;
typedef YShortcutList::iterator		YShortcutListIterator;


#endif // YShortcut_h
