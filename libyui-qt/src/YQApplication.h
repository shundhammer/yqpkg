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

  File:	      YQApplication.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQApplication_h
#define YQApplication_h

#include <qobject.h>
#include <qtranslator.h>
#include <qfont.h>

#include "YQPackageSelectorPluginStub.h"
#include "YQGraphPluginStub.h"
#include <yui/YApplication.h>

class QSettings;
class YQPackageSelectorPlugin;
class YQGraphPlugin;
class QMenu;


class YQApplication: public QObject, public YApplication
{
    Q_OBJECT

protected:

    friend class YQUI;

    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    YQApplication();

    /**
     * Destructor.
     **/
    virtual ~YQApplication();


public:

    /**
     * Set language and encoding for the locale environment ($LANG).
     *
     * Load UI-internal translations (e.g. for predefined dialogs like
     * file selection) and fonts.
     *
     * 'language' is the ISO short code ("de_DE", "en_US", ...).
     *
     * 'encoding' an (optional) encoding ("utf8", ...) that will be appended if
     *  present.
     *
     * Reimplemented from YApplication.
     **/
    virtual void setLanguage( const std::string & language,
			      const std::string & encoding = std::string() );

    /**
     * Load translations for Qt's predefined dialogs like file selection box
     * etc.
     **/
    void loadPredefinedQtTranslations();

    /**
     * Set the layout direction (left-to-right or right-to-left) from
     * 'language'.
     **/
    void setLayoutDirection( const std::string & language );

    /**
     * Set fonts according to the specified language and encoding.
     *
     * This is most important for some Asian languages that have overlaps in
     * the Unicode table, like Japanese vs. Chinese.
     **/
    void setLangFonts( const std::string & language,
		       const std::string & encoding = std::string() );

    /**
     * Returns the application's default font.
     **/
    const QFont & currentFont();

    /**
     * Returns the application's default bold font.
     **/
    const QFont & boldFont();

    /**
     * Returns the application's heading font.
     **/
    const QFont & headingFont();

    /**
     * Delete the fonts so they will be reloaded upon their next usage.
     **/
    void deleteFonts();

    /**
     * Determine good fonts based on defaultsize geometry and set
     * _auto_normal_font_size and _auto_heading_font_size accordingly.
     * Caches the values, so it's safe to call this repeatedly.
     **/
    void pickAutoFonts();

    /**
     * Returns 'true' if the UI  automatically picks fonts, disregarding Qt
     * standard settings.
     *
     * This makes sense during system installation system where the display DPI
     * cannot reliably be retrieved and thus Qt uses random font sizes based on
     * that random DPI.
     **/
    bool autoFonts() const { return _autoFonts; }

    /**
     * Set whether or not fonts should automatically be picked.
     **/
    void setAutoFonts( bool useAutoFonts );

    /**
     * Return a std::string for a named glyph.
     *
     * Using this is discouraged in new applications.
     *
     * Reimplemented from YApplication.
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
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForExistingDirectory( const std::string & startDir,
                                                 const std::string & headline );

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
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForExistingFile( const std::string & startWith,
                                            const std::string & filter,
                                            const std::string & headline );

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
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForSaveFileName( const std::string & startWith,
                                            const std::string & filter,
                                            const std::string & headline );
    /**
     * Lower-level version that works with QStrings and does not change
     * the mouse cursor.
     **/
    static QString askForSaveFileName( const QString & startWith,
				       const QString & filter,
				       const QString & headline );

    /**
     * Open a pop-up dialog to let the user select between the available widget
     * styles sheets (themes).
     *
     * This is also available in each dialog with Shift-F3; see YQDialog.cc.
     *
     * Reimplemented from YApplication.
     **/
    virtual void askForWidgetStyle();

    /**
     * Open a context menu for a widget
     *
     * 'itemCollection' describes the menu structure
     *
     * Returns true on success (otherwise false).
     *
     * Implemented from YApplication.
     **/
    virtual bool openContextMenu ( const YItemCollection & itemCollection );


    /**
     * Return the package selector plugin singleton or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    static YQPackageSelectorPluginStub * packageSelectorPlugin();

    /**
     * Return the package selector plugin singleton or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    static YQGraphPluginStub * graphPlugin();

    /**
     * A mouse click with the wrong mouse button was detected - e.g., a right
     * click on a push button. The user might be left-handed, but his mouse
     * might not (yet) be configured for left-handed use - e.g., during
     * installation. Ask him if he would like his mouse temporarily configured
     * as a left-handed mouse.
     *
     * This status can be queried with YQApplication::leftHandedMouse().
     **/
    void maybeLeftHandedUser();

    /**
     * Convert logical layout spacing units into device dependent units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Reimplemented from YApplication.
     **/
    virtual int deviceUnits( YUIDimension dim, float layoutUnits );

    /**
     * Convert device dependent units into logical layout spacing units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Reimplemented from YApplication.
     **/
    virtual float layoutUnits( YUIDimension dim, int deviceUnits );

    /**
     * Change the (mouse) cursor to indicate busy status.
     *
     * Reimplemented from YApplication.
     **/
    virtual void busyCursor();

    /**
     * Change the (mouse) cursor back from busy status to normal.
     *
     * Reimplemented from YApplication.
     **/
    virtual void normalCursor();

    /**
     * Make a screen shot and save it to the specified file.
     *
     * Reimplemented from YApplication.
     **/
    virtual void makeScreenShot( const std::string & fileName );

    /**
     * Beep.
     *
     * Reimplemented from YApplication.
     **/
    virtual void beep();

    /**
     * Return position of the context menu (in gloabl coordinates)
     **/
    virtual QPoint contextMenuPos() { return _contextMenuPos; }

    /**
     * Sets the position of the context menu (in gloabl coordinates)
     **/
    virtual void setContextMenuPos( QPoint contextMenuPos );

    // Display information and UI capabilities.
    //
    // All implemented from YApplication.

    virtual int	 displayWidth();
    virtual int	 displayHeight();
    virtual int	 displayDepth();
    virtual long displayColors();

    // Size of main dialogs
    virtual int	 defaultWidth();
    virtual int	 defaultHeight();

    //
    // UI capabilities
    //

    virtual bool isTextMode()			{ return false; }
    virtual bool hasImageSupport()		{ return true; }
    virtual bool hasIconSupport()		{ return true; }
    virtual bool hasAnimationSupport()		{ return true; }
    virtual bool hasFullUtf8Support()		{ return true; }
    virtual bool richTextSupportsTable()	{ return true; }
    virtual bool hasWizardDialogSupport()	{ return true; }
    virtual bool hasWidgetStyleSupport()        { return true; }
    virtual bool leftHandedMouse();

    /**
     * Set the application title
     *
     * Reimplemented from YApplication.
     **/
    virtual void setApplicationTitle(const std::string& title);

    /**
     * Set the application Icon
     *
     * Reimplemented from YApplication.
     **/
    virtual void setApplicationIcon(const std::string& icon);
protected:

    /**
     * Constructs a key for the language specific font file:
     *     "font[lang]"
     * for
     *     font[de_DE] = "Sans Serif"
     *     font[zh] = "ChineseSpecial, something"
     *     font[ja_JP] = "JapaneseSpecial, something"
     *     font = "Sans Serif"
     **/
    QString fontKey( const QString & lang );


    //
    // Data members
    //

    // Fonts

    QFont * _currentFont;
    QFont * _headingFont;
    QFont * _boldFont;

    /**
     * Font family or list of font families to use ("Sans Serif" etc.)
     **/
    QString _fontFamily;

    /**
     * Language-specific font settings
     **/
    QSettings	* _langFonts;

    /**
     * Translator for the predefined Qt dialogs
     **/
    QTranslator * _qtTranslations;

    //
    // Misc
    //

    bool _autoFonts;
    int  _autoNormalFontSize;
    int  _autoHeadingFontSize;

    bool _leftHandedMouse;
    bool _askedForLeftHandedMouse;

    QPoint _contextMenuPos;
    QMenu*  _contextMenu;


};


#endif // YQApplication_h
