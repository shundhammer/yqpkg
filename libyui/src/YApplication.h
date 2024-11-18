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

  File:		YApplication.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YApplication_h

#include <string>
#include <map>
#include "YUI.h"
#include "ImplPtr.h"
#include "YMenuItem.h"
#include  "YIconLoader.h"



class YWidget;
class YWidgetID;
struct YApplicationPrivate;


/**
 * Class for application-wide values and functions.
 * This is a singleton. Access and create it via the static functions in YUI.
 **/
class YApplication
{
protected:

    friend class YUI;
    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    YApplication();

    /**
     * Destructor.
     **/
    virtual ~YApplication();

public:

    /**
     * Find a widget in the topmost dialog by its ID.
     *
     * If there is no widget with that ID (or no dialog at all), this function
     * throws a YUIWidgetNotFoundException if 'doThrow' is 'true'. It returns 0
     * if 'doThrow' is 'false'.
     **/
    YWidget * findWidget( YWidgetID * id, bool doThrow = true ) const;

    /**
     * Get the base path for icons used by the UI. Selection widgets like
     * YSelectionBox, YComboBox, etc. or YWizard prepend this to icon
     * specifications that don't use an absolute path.
     **/
    virtual std::string iconBasePath() const;

    /**
     * Set the icon base path.
     **/
    virtual void setIconBasePath( const std::string & newIconBasePath );

    /**
     * Return the icon loader.
     **/
    YIconLoader * iconLoader();

    /**
     * Return the default function key number for a widget with the specified
     * label or 0 if there is none. Any keyboard shortcuts that may be
     * contained in 'label' are stripped away before any comparison.
     *
     * The basic idea behind this concept is to have an easy default mapping
     * from buttons etc. with the same semantics to function keys:
     *
     * "OK"	-> F10
     * "Accept"	-> F10
     * "Yes"	-> F10
     * "Next"	-> F10
     *
     * "Cancel"	-> F9
     * "No"	-> F9
     * ...
     *
     * This function returns 10 for F10, F for F9 etc.;
     * 0 means "no function key".
     **/
    int defaultFunctionKey( const std::string & label ) const;

    /**
     * Add a mapping from the specified label to the specified F-key number.
     * This is the counterpart to defaultFunctionKey().
     *
     * This only affects widgets that are created after this call.
     **/
    void setDefaultFunctionKey( const std::string & label, int fkey );

    /**
     * Clear all previous label-to-function-key mappings.
     **/
    void clearDefaultFunctionKeys();

    /**
     * Set language and encoding for the locale environment ($LANG).
     *
     * This affects UI-internal translations (e.g. for predefined dialogs like
     * file selection), encoding and fonts.
     *
     * 'language' is the ISO short code ("de_DE", "en_US", ...).
     *
     * 'encoding' an (optional) encoding ("utf8", ...) that will be appended if
     * present.
     *
     * Derived classes can overwrite this method, but they should call this
     * base class method at the beginning of the new implementation.
     **/
    virtual void setLanguage( const std::string & language,
			      const std::string & encoding = std::string() );

    /**
     * Return the current language from the locale environment ($LANG).
     * If 'stripEncoding' is true, any encoding (".utf8" etc.) is removed.
     **/
    std::string language( bool stripEncoding = false ) const;

    /**
     * Return a string for a named glyph:
     *
     *    YUIGlyph_ArrowLeft
     *    YUIGlyph_ArrowRight
     *    YUIGlyph_ArrowUp
     *    YUIGlyph_ArrowDown
     *    YUIGlyph_CheckMark
     *    YUIGlyph_BulletArrowRight
     *    YUIGlyph_BulletCircle
     *    YUIGlyph_BulletSquare
     *
     * Using this is discouraged in new applications.
     * This method is available for backward compatibility.
     *
     * This default implementation returns simple textual representations for
     * each glyph simbol (e.g., "->" for YUIGlyphArrorRight).
     *
     * Derived classes are free to overwrite this. It does not make sense to
     * call this base class method in a new implementation.
     **/
    virtual std::string glyph( const std::string & glyphSymbolName );

    /**
     * Open a directory selection box and prompt the user for an existing
     * directory.
     *
     * 'startDir' is the initial directory that is displayed.
     *
     * 'headline' is an explanatory text for the directory selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected directory name
     * or an empty string if the user canceled the operation.
     *
     * Derived classes are required to implement this.
     **/
    virtual std::string askForExistingDirectory( const std::string & startDir,
                                                 const std::string & headline ) = 0;

    /**
     * Open a file selection box and prompt the user for an existing file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     *
     * Derived classes are required to implement this.
     **/
    virtual std::string askForExistingFile( const std::string & startWith,
                                            const std::string & filter,
                                            const std::string & headline ) = 0;

    /**
     * Open a file selection box and prompt the user for a file to save data
     * to.  Automatically asks for confirmation if the user selects an existing
     * file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     *
     * Derived classes are required to implement this.
     **/
    virtual std::string askForSaveFileName( const std::string & startWith,
                                            const std::string & filter,
                                            const std::string & headline ) = 0;

    /**
     * Open a pop-up dialog to let the user select between the available widget
     * styles (themes).
     *
     * This default implementation does nothing. Query with
     * hasWidgetStyleSupport() if the UI provides that capability.
     *
     * Derived classes are free to overwrite this, but then they should also
     * overwrite hasWidgetStyleSupport().
     **/
    virtual void askForWidgetStyle() {}

    /**
     * Open a context menu for a widget
     *
     * 'itemCollection' describes the menu structure
     *
     * Returns true on success (otherwise false).
     *
     * Derived classes are free to overwrite this.
     **/
    virtual bool openContextMenu( const YItemCollection & itemCollection );


    /**
     * Set the current product name ("openSUSE", "SLES", ...).
     * This name will be expanded in help texts when the &product; entity is
     * used.
     *
     * Derived classes can overwrite this method, but they should call this
     * base class method in the new implementation.
     **/
    virtual void setProductName( const std::string & productName );

    /**
     * Get the current product name ("openSUSE", "SLES", ...).
     **/
    std::string productName() const;

    /**
     * Set release notes; map product => text
     *
     */
    void setReleaseNotes( const std::map<std::string,std::string> & relNotes );

    /**
     * Get the current release notes map
     **/
    std::map<std::string,std::string> releaseNotes() const;

    /**
     * Set whether the product logo (in top bar) should be shown
     */
    void setShowProductLogo( bool show );

    /**
     * Return true if product logo should be shown
     */
    bool showProductLogo() const;

    /**
     * Convert logical layout spacing units into device dependent units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Derived classes may want to reimplement this method.
     **/
    virtual int deviceUnits( YUIDimension dim, float layoutUnits );

    /**
     * Convert device dependent units into logical layout spacing units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Derived classes may want to reimplement this method.
     **/
    virtual float layoutUnits( YUIDimension dim, int deviceUnits );

    /**
     * Set reverse layout for Arabic / Hebrew support.
     *
     * Derived classes can overwrite this method, but they should call this
     * base class method in the new implementation.
     **/
    virtual void setReverseLayout( bool reverse );

    /**
     * Returns 'true' if widget geometry should be reversed for languages that
     * have right-to-left writing direction (Arabic, Hebrew).
     **/
    bool reverseLayout() const;

    /**
     * Change the (mouse) cursor to indicate busy status.
     * This default implementation does nothing.
     **/
    virtual void busyCursor() {}

    /**
     * Change the (mouse) cursor back from busy status to normal.
     * This default implementation does nothing.
     **/
    virtual void normalCursor() {}

    /**
     * Make a screen shot and save it to the specified file.
     * This default implementation does nothing.
     **/
    virtual void makeScreenShot( const std::string & fileName ) {}

    /**
     * Beep.
     * This default implementation does nothing.
     **/
    virtual void beep() {}


    //
    // NCurses (text mode) specific
    //

    /**
     * Redraw the screen.
     * This default implementation does nothing.
     **/
    virtual void redrawScreen() {}

    /**
     * Initialize the (text) console keyboard.
     * This default implementation does nothing.
     **/
    virtual void initConsoleKeyboard() {}

    /**
     * Set the (text) console font according to the current encoding etc.
     * See the setfont(8) command and the console HowTo for details.
     *
     * This default implementation does nothing.
     **/
    virtual void setConsoleFont( const std::string & console_magic,
				 const std::string & font,
				 const std::string & screen_map,
				 const std::string & unicode_map,
				 const std::string & language )
	{}

    /**
     * Run a shell command (typically an interactive program using NCurses)
     * in a terminal (window).
     *
     * This is useful for text UIs (e.g., NCurses) that need special
     * preparation prior to running an NCurses-based application and special
     * clean-up afterwards.
     *
     * This default implementation logs an error and returns -1.
     **/
    virtual int runInTerminal( const std::string & command );


    /// @{
    /**
     * To mix TUI (NCurses) with stdio, enclose the UI parts
     * within openUI/closeUI
     *
     * This default implementation does nothing.
     */
    virtual void openUI() {}
    virtual void closeUI() {}
    /// @}

    //
    // Display information.
    //
    // Width and height are returned in the the UI's native dimension:
    // Pixels for graphical UIs, character cells for text UIs.
    // -1 means "value cannot be obtained" for int functions.
    //
    // Derived classes are required to implement these functions.
    //

    virtual int	 displayWidth()			= 0;
    virtual int	 displayHeight()		= 0;
    virtual int	 displayDepth()			= 0;
    virtual long displayColors()		= 0;

    // Size of main dialogs
    virtual int	 defaultWidth()			= 0;
    virtual int	 defaultHeight()		= 0;

    //
    // UI capabilities
    //

    virtual bool isTextMode()			= 0;
    virtual bool hasImageSupport()		= 0;
    virtual bool hasIconSupport()		= 0;
    virtual bool hasAnimationSupport()		= 0;
    virtual bool hasFullUtf8Support()		= 0;
    virtual bool richTextSupportsTable()	= 0;
    virtual bool leftHandedMouse()		= 0;
    virtual bool hasWidgetStyleSupport()  { return false; }
    virtual bool hasWizardDialogSupport() { return false; }


    /**
     * Set the application title
     **/
    virtual void setApplicationTitle ( const std::string& title );

    /**
     * Get the application title
     *
     * Default title is the running command (argv[0])
     **/
    virtual const std::string& applicationTitle() const;

    /**
     * Set the application Icon
     **/
    virtual void setApplicationIcon ( const std::string& icon );

    /**
     * Get the application Icon
     *
     * Default icon is an empty string
     **/
     virtual const std::string& applicationIcon() const;

private:

    ImplPtr<YApplicationPrivate> priv;

};

#define YApplication_h

#endif // YApplication_h
