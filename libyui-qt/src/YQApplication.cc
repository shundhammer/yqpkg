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

  File:		YQApplication.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

  Textdomain	"qt"
/-*/

#include <unistd.h>	// access()

#include <QApplication>
#include <QLocale>
#include <QRegExp>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSettings>
#include <QFontDatabase>
#include <QMenu>
#include <QLibraryInfo>

#include <fontconfig/fontconfig.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <yui/YUISymbols.h>

#include "YQUI.h"

#include "utf8.h"
#include "YQi18n.h"

#include "YQApplication.h"
#include "YQContextMenu.h"
#include "YQDialog.h"
#include "YQPackageSelectorPluginStub.h"
#include "YQGraphPluginStub.h"
#include "QY2StyleSheetSelector.h"


// Allow overriding on the compile command line with -DLANG_FONTS_FILE=/foo

#ifndef LANG_FONTS_FILE
#  define LANG_FONTS_FILE "/usr/share/libyui/data/lang_fonts"
#endif

#ifndef ICON_DIR
#  define ICON_DIR "/usr/share/YaST2/theme/current/icons"
#endif


using std::string;
using std::endl;


// Qt5 requires explicit font initialization; otherwise it picks up
// any random matching fonts, and tends to choose the worst one
// (e.g. bitmap fonts) in the end. (bnc#879991)
// Note that this is also set in LANG_FONTS_FILE
static const char * default_font_family = "Sans Serif";



YQApplication::YQApplication()
    : YApplication()
    , _currentFont( 0 )
    , _headingFont( 0 )
    , _boldFont( 0 )
    , _langFonts( 0 )
    , _qtTranslations( 0 )
    , _autoFonts( false )
    , _autoNormalFontSize( -1 )
    , _autoHeadingFontSize( -1 )
    , _leftHandedMouse( false )
    , _askedForLeftHandedMouse( false )
    , _contextMenuPos ( QPoint (0, 0) )
    , _contextMenu ( 0 )
{
    yuiDebug() << "YQApplication constructor start" << endl;

    yuiMilestone() << "QIcon::themeName = '" << QIcon::themeName() << "'" << endl;

    iconLoader()->addIconSearchPath( ICON_DIR );
    loadPredefinedQtTranslations();
    _fontFamily = default_font_family;

    yuiDebug() << "YQApplication constructor end" << endl;
}


YQApplication::~YQApplication()
{
    delete _langFonts;
    delete _qtTranslations;

    deleteFonts();
}

static string glob_language = "";

void
YQApplication::setLanguage( const string & language,
			    const string & encoding )
{
    glob_language = language;
    YApplication::setLanguage( language, encoding );
    loadPredefinedQtTranslations();

    bool oldReverseLayout = YApplication::reverseLayout();
    setLayoutDirection( language );
    setLangFonts( language, encoding );

    if ( oldReverseLayout != YApplication::reverseLayout() )
    {
	YDialog * dialog = YDialog::topmostDialog( false ); // don't throw

	if ( dialog )
	    dialog->recalcLayout();
    }
}


void
YQApplication::loadPredefinedQtTranslations()
{
    QString path = QLibraryInfo::location( QLibraryInfo::TranslationsPath );
    QString language;

    if (glob_language == "")
        language = QLocale::system().name();
    else
        language = glob_language.c_str();

    QString transFile = QString( "qt_%1.qm").arg( language );

    yuiMilestone() << "Selected language: " << language << endl;

    if ( path.isEmpty() )
    {
	yuiWarning() << "Qt locale directory not set - "
		     << "no translations for predefined Qt dialogs"
		     << endl;
	return;
    }

    if ( ! _qtTranslations )
	_qtTranslations = new QTranslator();

    if ( !_qtTranslations->load( transFile, path ) )
    {
	yuiWarning() << "Can't load translations for predefined Qt dialogs for "
		     << language << endl;
    }
    else
    {
	yuiMilestone() << "Loaded translations for predefined Qt dialogs for "
		       << language << endl;

	qApp->installTranslator( _qtTranslations );

	if ( qApp->layoutDirection() == Qt::RightToLeft )
	    YApplication::setReverseLayout( true );
    }
}


void
YQApplication::setLayoutDirection( const string & language )
{
    QString lang( language.c_str() );

    // Force reverse layout for Arabic, Farsi and Hebrew.
    // Alternatively, we could use QLocale::textDirection().

    if ( lang.startsWith( "ar" ) ||	// Arabic
	 lang.startsWith( "fa" ) ||	// Farsi
	 lang.startsWith( "he" ) )	// Hebrew
    {
	yuiMilestone() << "Using reverse layout for " << language << endl;

	qApp->setLayoutDirection( Qt::RightToLeft );
	YApplication::setReverseLayout( true );
    }
    else
    {
	qApp->setLayoutDirection( Qt::LeftToRight );
	YApplication::setReverseLayout( false );
    }

    // Qt tries to figure that out by having translators translate a message
    // "QT_LAYOUT_DIRECTION" to "RTL" for right-to-left languages (i.e.,
    // Arabic, Hebrew) with QQapplication::tr(). This of course only works if
    // there are translations for those languages for QTranslator in the first
    // place, i.e. it only works if translations for the predefined Qt dialogs
    // (file selection dialog etc.) are available - and being loaded.
    //
    // libqt4-x11 contains Arabic translations for those Qt standard dialogs in
    // /usr/share/qt4/translations/qt_ar.qm, but (as of Sept. 2008) no Hebrew
    // translations.
    //
    // Anyway, that Qt standard way is not very reliable. And they only do it
    // at program startup anyway. Any later loading of those translations will
    // not help.
}


void
YQApplication::setLangFonts( const string & language, const string & encoding )
{
    if ( ! _langFonts )
    {
	_langFonts = new QSettings( LANG_FONTS_FILE, QSettings::IniFormat );
	Q_CHECK_PTR( _langFonts );

	if ( _langFonts->status() != QSettings::NoError )
	    yuiError() << "Error reading " << _langFonts->fileName() << endl;
	else
	    yuiMilestone() <<  _langFonts->fileName() << " read OK"
			   << qPrintable( _langFonts->allKeys().join( "-" ) )
			   << endl;
    }

    QString lang = language.c_str();

    if ( ! encoding.empty() )
	lang += QString( "." ) + encoding.c_str();

    QString key;
    bool reloadFont = false;

    if ( ! _langFonts->contains( fontKey( lang ) ) )	// Try with encoding ("zh_CN.UTF8" etc.)
    {
	lang = language.c_str();			// Try without encoding ("zh_CN")

	if ( ! _langFonts->contains( fontKey( lang ) ) )
	    lang.replace( QRegExp( "_.*$" ), "" );	// Cut off trailing country ("_CN")
    }

    if ( _langFonts->contains( fontKey( lang ) ) )
    {
	QStringList fontList =
	    _langFonts->value( fontKey( lang ), "" ).toString().split( "," );
	for ( int i = 0; i < fontList.size(); ++i )
	{
	    yuiMilestone() << fontKey( lang ) << " adding " << fontList.at( i ) << endl;
	    QFontDatabase::addApplicationFont( fontList.at( i ) );
	}

	reloadFont = true;
    }

    if ( _fontFamily.isEmpty() )
    {
        _fontFamily = default_font_family;
	reloadFont = true;
    }

    if (reloadFont)
    {
	yuiMilestone() << "Reloading fonts" << endl;

	// update fonts
	deleteFonts();

        foreach ( QWidget *widget, QApplication::allWidgets() )
        {
            QFont wfont( widget->font() );
            wfont.setFamily( _fontFamily );
            widget->setFont( wfont );
        }

        QFont font( qApp->font() );
        font.setFamily( _fontFamily );
        qApp->setFont(font);	// font, informWidgets

	yuiMilestone() << "Removing the key " << lang << endl;
	_langFonts->remove( fontKey( lang ) );
    }
    else
    {
	yuiDebug() << "No font change" << endl;
    }

}


QString
YQApplication::fontKey( const QString & lang )
{
    if ( lang.isEmpty() )
	return "font";
    else
	return QString( "font[%1]").arg( lang );
}


const QFont &
YQApplication::currentFont()
{
    /**
     * Brute force approach to make sure we'll really get a complete Unicode font:
     * Explicitly load the one font that we made sure to contain all required
     * characters, including Latin1, Latin2, Japanese, Korean, and the
     * characters used for glyphs.
     *
     * There are many fonts that claim to be Unicode, but most of them contain
     * just a sorry excuse for a complete Unicode character set. Qt can't know
     * how complete a font is, so it chooses one that might be better in otherf
     * aspects, but lacks necessary characters.
     **/

    if ( ! _currentFont )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _currentFont = new QFont( _fontFamily );
	    _currentFont->setPixelSize( _autoNormalFontSize );
	    _currentFont->setWeight( QFont::Normal );

	    yuiMilestone() << "Loaded " <<  _autoNormalFontSize
			   << " pixel font: " << _currentFont->toString()
			   << endl;

	    qApp->setFont( * _currentFont);	// font, informWidgets
	}
	else
	{
	    // yuiDebug() << "Copying QApplication::font()" << endl;
	    _currentFont = new QFont( qApp->font() );
	}
    }

    return * _currentFont;
}


const QFont &
YQApplication::boldFont()
{
    if ( ! _boldFont )
    {
	_boldFont = new QFont( currentFont() );
	_boldFont->setBold( true );
    }

    return * _boldFont;
}


const QFont &
YQApplication::headingFont()
{
    /**
     * Brute force load the heading font - see currentFont() above for more.
     **/

    if ( ! _headingFont )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _headingFont = new QFont( _fontFamily );
	    _headingFont->setPixelSize( _autoHeadingFontSize );
	    _headingFont->setWeight( QFont::Bold );

	    yuiMilestone() << "Loaded " << _autoHeadingFontSize
			   << " pixel bold font: " << _headingFont->toString()
			   << endl;
	}
	else
	{
	    _headingFont = new QFont( _fontFamily, 14, QFont::Bold );
	}
    }

    return * _headingFont;
}


void
YQApplication::deleteFonts()
{
    delete _currentFont;
    delete _headingFont;
    delete _boldFont;

    _currentFont = 0;
    _headingFont = 0;
    _boldFont	 = 0;
}


void
YQApplication::setAutoFonts( bool useAutoFonts )
{
    _autoFonts = useAutoFonts;
}


void
YQApplication::pickAutoFonts()
{
    if ( _autoNormalFontSize >= 0 )	// Use cached values
	return;

    int x = defaultWidth();
    int y = defaultHeight();

    int normal	= 10;
    int heading = 12;

    if ( x >= 800 && y >= 600 )
    {
	normal	= 10;
	heading = 12;
    }

    if ( x >= 1024 && y >= 768 )
    {
	normal	= 12;
	heading = 14;
    }

    if ( x >= 1280 && y >= 1024 )
    {
	normal	= 14;
	heading = 18;
    }

    if ( x >= 1400 )
    {
	normal	= 16;
	heading = 20;
    }

    if ( x >= 1600 )
    {
	normal	= 18;
	heading = 24;
    }

    if ( x >= 2048 )	// Sounds futuristic? Just wait one or two years...
    {
	normal	= 20;
	heading = 28;
    }

    _autoNormalFontSize	 = normal;
    _autoHeadingFontSize = heading;

    yuiMilestone() << "Selecting auto fonts - normal: " << _autoNormalFontSize
		   << ", heading: " <<  _autoHeadingFontSize  << " (bold)"
		   << endl;
}


string
YQApplication::glyph( const string & sym )
{
    QChar unicodeChar;

    // Hint: Use the 'xfd' program to view characters available in the Unicode font.

    if      ( sym == YUIGlyph_ArrowLeft         ) 	unicodeChar = QChar( reverseLayout() ? 0x2192 : 0x2190 );
    else if ( sym == YUIGlyph_ArrowRight        )       unicodeChar = QChar( reverseLayout() ? 0x2190 : 0x2192 );
    else if ( sym == YUIGlyph_ArrowUp           )       unicodeChar = QChar( 0x2191 );
    else if ( sym == YUIGlyph_ArrowDown         )       unicodeChar = QChar( 0x2193 );
    else if ( sym == YUIGlyph_CheckMark         )       unicodeChar = QChar( 0x2714 );
    else if ( sym == YUIGlyph_BulletArrowRight  )       unicodeChar = QChar( 0x279c );
    else if ( sym == YUIGlyph_BulletCircle      )       unicodeChar = QChar( 0x274d );
    else if ( sym == YUIGlyph_BulletSquare      )       unicodeChar = QChar( 0x274f );
    else return "";

    return toUTF8( QString( unicodeChar ) );
}


string
YQApplication::askForExistingDirectory( const string & startDir,
					const string & headline )
{
    normalCursor();

    QString dirName =
	QFileDialog::getExistingDirectory( 0,				// parent
	 				   fromUTF8( headline ) ,	// caption
					   fromUTF8( startDir ), QFileDialog::DontUseNativeDialog);	// dir

    busyCursor();

    return toUTF8( dirName );
}


string
YQApplication::askForExistingFile( const string & startWith,
				   const string & filter,
				   const string & headline )
{
    normalCursor();

    QFileDialog* dialog = new QFileDialog( 0,				// parent
                                           fromUTF8( headline ),	// caption
                                           fromUTF8( startWith ),	// dir
                                           fromUTF8( filter ));		// filter
    dialog->setFileMode( QFileDialog::ExistingFile );
    dialog->setFilter( QDir::System | dialog->filter() );
    dialog->setOptions( QFileDialog::DontUseNativeDialog );

    QString fileName;
    if ( dialog->exec() == QDialog::Accepted )
        fileName = dialog->selectedFiles().value( 0 );
    delete dialog;

    busyCursor();

    return toUTF8( fileName );
}


string
YQApplication::askForSaveFileName( const string & startWith,
				   const string & filter,
				   const string & headline )
{
    normalCursor();

    QString fileName = askForSaveFileName( fromUTF8( startWith ),
					   fromUTF8( filter ),
					   fromUTF8( headline ) );
    busyCursor();

    return toUTF8( fileName );
}


QString
YQApplication::askForSaveFileName( const QString & startWith,
				   const QString & filter,
				   const QString & headline )
{
    QString fileName;

    // Leave the mouse cursor alone - this function might be called from
    // some other widget, not only from UI::AskForSaveFileName().

    fileName = QFileDialog::getSaveFileName( YQDialog::popupParent(),
                                             headline,		// caption
                                             startWith,		// dir
                                             filter, 0, QFileDialog::DontUseNativeDialog ); // filter

    if ( fileName.isEmpty() )	// this includes fileName.isNull()
	return QString();

    return fileName;
}


void
YQApplication::askForWidgetStyle()
{
    normalCursor();

    QY2StyleSheetSelector dialog( YQDialog::popupParent() );
    dialog.exec();
    // The return code doesn't matter because the dialog applies any changes instantly.

    busyCursor();
}


bool
YQApplication::openContextMenu( const YItemCollection & itemCollection )
{
    YQContextMenu* menu = new YQContextMenu( YQDialog::popupParent(), _contextMenuPos );
    menu->addItems(itemCollection);

    return true;
}


int
YQApplication::displayWidth()
{
    return qApp->desktop()->width();
}


int
YQApplication::displayHeight()
{
    return qApp->desktop()->height();
}


int
YQApplication::displayDepth()
{
    return qApp->desktop()->depth();
}


long
YQApplication::displayColors()
{
    return 1L << qApp->desktop()->depth();
}


int
YQApplication::defaultWidth()
{
    return YQUI::ui()->defaultSize( YD_HORIZ );
}


int
YQApplication::defaultHeight()
{
    return YQUI::ui()->defaultSize( YD_VERT );
}


bool
YQApplication::leftHandedMouse()
{
    return _leftHandedMouse;
}


void
YQApplication::maybeLeftHandedUser()
{
    if ( _askedForLeftHandedMouse )
	return;

    QString message =
	_( "You clicked the right mouse button "
	   "where a left-click was expected."
	   "\n"
	   "Switch left and right mouse buttons?"
	   );

    int button = QMessageBox::question( YQDialog::popupParent(),
					// Popup dialog caption
					_( "Unexpected Click" ),
					message,
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No,
					QMessageBox::Cancel | QMessageBox::Escape );

    if ( button == QMessageBox::Yes )
    {
        int result;
	const char * command =
	    _leftHandedMouse ?
	    "xmodmap -e \"pointer = 1 2 3\"":	// switch back to right-handed mouse
	    "xmodmap -e \"pointer = 3 2 1\"";	// switch to left-handed mouse

	_leftHandedMouse	 = ! _leftHandedMouse; 	// might be set repeatedly!
	_askedForLeftHandedMouse = false;	// give the user a chance to switch back
	yuiMilestone() << "Switching mouse buttons: " << command << endl;

	result = system( command );
        if (result < 0)
            yuiError() << "Calling '" << command << "' failed" << endl;
        else if (result > 0)
            yuiError() << "Running '" << command << "' exited with " << result << endl;
    }
    else if ( button == 1 )	// No
    {
	_askedForLeftHandedMouse = true;
    }
}


int YQApplication::deviceUnits( YUIDimension dim, float layoutUnits )
{
    if ( dim==YD_HORIZ )	layoutUnits *= ( 640.0/80 );
    else			layoutUnits *= ( 480.0/25 );

    return (int) ( layoutUnits + 0.5 );
}


float YQApplication::layoutUnits( YUIDimension dim, int deviceUnits )
{
    float size = (float) deviceUnits;

    if ( dim==YD_HORIZ )	size *= ( 80/640.0 );
    else			size *= ( 25/480.0 );

    return size;
}


void YQApplication::beep()
{
    qApp->beep();
}


void YQApplication::busyCursor()
{
    YQUI::ui()->busyCursor();
}


void YQApplication::normalCursor()
{
    YQUI::ui()->normalCursor();
}


void YQApplication::makeScreenShot( const string & fileName )
{
    YQUI::ui()->makeScreenShot( fileName );
}


YQPackageSelectorPluginStub *
YQApplication::packageSelectorPlugin()
{
    static YQPackageSelectorPluginStub * plugin = 0;

    if ( ! plugin )
    {
        plugin = new YQPackageSelectorPluginStub();

        // This is a deliberate memory leak: If an application requires a
        // PackageSelector, it is a package selection application by
        // definition. In this case, the ncurses_pkg plugin is intentionally
        // kept open to avoid repeated start-up cost of the plugin and libzypp.
    }

    return plugin;
}


YQGraphPluginStub *
YQApplication::graphPlugin()
{
    static YQGraphPluginStub * plugin = 0;

    if ( ! plugin )
    {
        plugin = new YQGraphPluginStub();

        // This is a deliberate memory leak: Plugin is intentionally
        // kept open to avoid repeated start-up cost of the plugin.
    }

    return plugin;
}


void
YQApplication::setContextMenuPos( QPoint contextMenuPos )
{
    _contextMenuPos = contextMenuPos;
}


void YQApplication::setApplicationTitle ( const string & title )
{
    QString qtTitle = fromUTF8( title );
    YApplication::setApplicationTitle ( title );
    YQUI::ui()->setApplicationTitle(qtTitle);
    qApp->setApplicationName(qtTitle);
}


void YQApplication::setApplicationIcon ( const string & icon )
{
    QString qtIcon = fromUTF8( icon );
    YApplication::setApplicationIcon ( icon );
    QString icon_name = QFileInfo( qtIcon ).baseName();

    if ( QIcon::hasThemeIcon( icon_name ) )
    {
        qApp->setWindowIcon( QIcon::fromTheme ( icon_name ) );
    }
    else
    {
        QPixmap pixmap( qtIcon );

        if ( !pixmap.isNull() )
            qApp->setWindowIcon( QIcon( pixmap ) );
    }
}
