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

  File:		YApplication.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <locale.h> 	// setlocale()
#include <map>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YApplication.h"
#include "YDialog.h"
#include "YUIException.h"
#include "YShortcut.h"
#include "YUI.h"
#include "YItem.h"
#include "YCommandLine.h"

using std::string;
using std::map;

typedef map<string, int> YFunctionKeyMap;


struct YApplicationPrivate
{
    YApplicationPrivate()
	: productName( "SUSE Linux" )
	, reverseLayout( false )
	, showProductLogo( false )
	{}

    string		productName;
    bool		reverseLayout;
    string              applicationTitle;
    string              applicationIcon;
    YFunctionKeyMap	defaultFunctionKey;
    YIconLoader*	iconLoader;
    map<string,string>	releaseNotes;
    bool		showProductLogo;
};


YApplication::YApplication()
    : priv( new YApplicationPrivate() )
{
    YUI_CHECK_NEW( priv );
    priv->iconLoader = new YIconLoader();
    YCommandLine cmdLine; // Retrieve command line args from /proc/<pid>/cmdline
    if ( cmdLine.argc() > 0 )
      priv->applicationTitle = cmdLine.arg(0);
}


YApplication::~YApplication()
{
    // NOP
}


YWidget *
YApplication::findWidget( YWidgetID * id, bool doThrow ) const
{
    YDialog * dialog = YDialog::currentDialog( doThrow );

    if ( ! dialog ) // has already thrown if doThrow == true
	return 0;

    return dialog->findWidget( id, doThrow );
}


string
YApplication::iconBasePath() const
{
    return priv->iconLoader->iconBasePath();
}


void
YApplication::setIconBasePath( const string & newIconBasePath )
{
    priv->iconLoader->setIconBasePath ( newIconBasePath );
}

YIconLoader *
YApplication::iconLoader()
{
   return priv->iconLoader;
}

void
YApplication::setProductName( const string & productName )
{
    priv->productName = productName;
}


string
YApplication::productName() const
{
    return priv->productName;
}

void
YApplication::setReleaseNotes( const map<string,string> & relNotes )
{
    priv->releaseNotes = relNotes;
}

map<string,string>
YApplication::releaseNotes() const
{
    return priv->releaseNotes;
}

void
YApplication::setShowProductLogo( bool show )
{
    priv->showProductLogo = show;
}

bool
YApplication::showProductLogo() const
{
    return priv->showProductLogo;
}

void
YApplication::setReverseLayout( bool reverse )
{
    priv->reverseLayout = reverse;
}


bool YApplication::reverseLayout() const
{
    return priv->reverseLayout;
}


int
YApplication::defaultFunctionKey( const string & label ) const
{
    YFunctionKeyMap::const_iterator result =
	priv->defaultFunctionKey.find( YShortcut::cleanShortcutString( label  ) );

    if ( result == priv->defaultFunctionKey.end() )
	return 0;
    else
	return result->second;
}


void
YApplication::setDefaultFunctionKey( const string & label, int fkey )
{
    if ( fkey > 0 )
	priv->defaultFunctionKey[ YShortcut::cleanShortcutString( label ) ] = fkey;
    else
	YUI_THROW( YUIException( "Bad function key number" ) );
}


void
YApplication::clearDefaultFunctionKeys()
{
    priv->defaultFunctionKey.clear();
}


void
YApplication::setLanguage( const string & language, const string & encoding )
{
    string lang = language;

    if ( ! encoding.empty() )
    {
	lang += ".";
	lang += encoding;
    }

    setenv( "LANG", lang.c_str(), 1 );  // 1 : replace
    setlocale( LC_NUMERIC, "C" );	// but always format numbers with "."

    yuiMilestone() <<  "Setting language to " << lang << endl;
}


string
YApplication::language( bool stripEncoding ) const
{
    const char *lang_env = getenv( "LANG" );

    if ( ! lang_env )
	return "";

    string lang( lang_env );

    if ( stripEncoding )
    {
	string::size_type pos = lang.find_first_of( ".@" );

	if ( pos != string::npos )		// if encoding etc. specified
	{
	    lang = lang.substr( 0, pos );	// remove it
	}
    }

    return lang;
}


string
YApplication::glyph( const string & sym )
{
    if	    ( sym == YUIGlyph_ArrowLeft		)	return ( reverseLayout() ? "->" : "<-"  );
    else if ( sym == YUIGlyph_ArrowRight	)	return ( reverseLayout() ? "<-" : "->"  );
    else if ( sym == YUIGlyph_ArrowUp		)	return ( "^"   );
    else if ( sym == YUIGlyph_ArrowDown		)	return ( "v"   );
    else if ( sym == YUIGlyph_CheckMark		)	return ( "x"   );
    else if ( sym == YUIGlyph_BulletArrowRight	)	return ( "=>"  );
    else if ( sym == YUIGlyph_BulletCircle	)	return ( "o"   );
    else if ( sym == YUIGlyph_BulletSquare	)	return ( "[]"  );
    else	// unknown glyph symbol
    {
	yuiError() << "Unknown glyph :" << sym << endl;
	return "";
    }
}

bool
YApplication::openContextMenu( const YItemCollection & itemCollection )
{
    YUI_THROW( YUIUnsupportedWidgetException( "ContextMenu" ) );
    return false;
}



int
YApplication::deviceUnits( YUIDimension dim, float layoutUnits )
{
    return (int) ( layoutUnits + 0.5 );
}


float
YApplication::layoutUnits( YUIDimension dim, int deviceUnits )
{
    return (float) deviceUnits;
}


int
YApplication::runInTerminal ( const string & module )
{
    yuiError() << "Not in text mode: Cannot run external program in terminal." << endl;

    return -1;
}

void YApplication::setApplicationTitle(const string &title)
{
    priv->applicationTitle = title;
}

const string &YApplication::applicationTitle() const
{
    return priv->applicationTitle;
}

void YApplication::setApplicationIcon(const string &icon)
{
    priv->applicationIcon = icon;
}
const string &YApplication::applicationIcon() const
{
    return priv->applicationIcon;
}

