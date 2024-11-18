/*
  Copyright (c) 2012 Björn Esser

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
  SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


/*-/

  File:		YSettings.cc

  Author:	Björn Esser <bjoern.esser@gmail.com>

  /-*/

#include "YSettings.h"
#include "YUIException.h"

#define YUILogComponent "ui"
#include "YUILog.h"
#include "Libyui_config.h"

using std::string;

string  YSettings::_progDir = "";
string  YSettings::_iconDir = "";
string  YSettings::_themeDir = "";
string  YSettings::_localeDir = "";
string  YSettings::_loadedUI = "";


YSettings::YSettings()
{
}


YSettings::~YSettings ()
{
}

void YSettings::setProgDir( string directory )
{
    if ( _progDir.empty() )
    {
        _progDir = directory;
        yuiDebug () << "Set progDir to \"" << directory << "\"" << endl;
        yuiDebug () << "progDir is now locked." << endl;
    }
    else
    {
        yuiDebug () << "Can't set progDir to \"" << directory << "\"" << endl;
        yuiDebug () << "It is locked to: \"" << _progDir << "\"" << endl;
        YUI_THROW ( YUIException ( "progSubDir is locked to: \"" + _progDir + "\"" ) );
    }
}

string YSettings::progDir ()
{
    // yuiDebug () << "progDir: \"" << _progDir << "\"" << endl;

    return _progDir;
}


void YSettings::setIconDir( string directory )
{
    if ( _iconDir.empty() )
    {
        _iconDir = directory;
        yuiDebug () << "Set iconDir to \"" << directory << "\"" << endl;
        yuiDebug () << "iconDir is now locked." << endl;
    }
    else
    {
        yuiDebug () << "Can't set iconDir to \"" << directory << "\"" << endl;
        yuiDebug () << "It is locked to: \"" << _iconDir << "\"" << endl;
        YUI_THROW ( YUIException ( "progIconDir is locked to: \"" + _iconDir + "\"" ) );
    }
}


string YSettings::iconDir ()
{
    if (_iconDir.size())
    {
        yuiDebug () << "iconDir: \"" << _iconDir << "\"" << endl;
        return _iconDir;
    }
    else if (_progDir.size())
        return _progDir + "/icons/";

    return THEMEDIR "/icons/";
}


void YSettings::setThemeDir( string directory )
{
    if ( _themeDir.empty() )
    {
        _themeDir = directory;
        yuiDebug () << "Set themeDir to \"" << directory << "\"" << endl;
        yuiDebug () << "themeDir is now locked." << endl;
    }
    else
    {
        yuiDebug () << "Can't set themeDir to \"" << directory << "\"" << endl;
        yuiDebug () << "It is locked to: \"" << _themeDir << "\"" << endl;
        YUI_THROW ( YUIException ( "themeDir is locked to: \"" + _themeDir + "\"" ) );
    }
}


string YSettings::themeDir ()
{
    if ( _themeDir.size() )
    {
        yuiDebug () << "themeDir: \"" << _themeDir << "\"" << endl;
        return _themeDir;
    }
    else if ( _progDir.size() )
    {
        // backwards compatibility if setProgSubDir is set to "/usr/share/YaST2"
        return _progDir + "/theme/current/wizard/";
    }

    return THEMEDIR "/current/wizard/";
}


void YSettings::setLocaleDir( string directory )
{
    if ( _localeDir.empty() )
    {
        _localeDir = directory;
        yuiDebug () << "Set localeDir to \"" << directory << "\"" << endl;
        yuiDebug () << "localeDir is now locked." << endl;
    }
    else
    {
        yuiDebug () << "Can't set localeDir to \"" << directory << "\"" << endl;
        yuiDebug () << "It is locked to: \"" << _localeDir << "\"" << endl;
        YUI_THROW ( YUIException ( "localeDir is locked to: \"" + _localeDir + "\"" ) );
    }
}

string YSettings::localeDir ()
{
    if ( _localeDir.size() )
    {
        yuiDebug () << "localeDir: \"" << _localeDir << "\"" << endl;
        return _localeDir;
    }
    else if ( _progDir.size() )
    {
        // backwards compatibility if ProgDir is set to "/usr/share/YaST2"
        return _progDir + "/locale/";
    }

    return "/usr/share/locale/";
}

void YSettings::loadedUI( string ui, bool force )
{
    if ( _loadedUI.empty() || force )
    {
        _loadedUI = ui;
        yuiDebug () << "Set loadedUI to \"" << ui << "\"" << endl;
        yuiDebug () << "loadedUI is now locked." << endl;
    }
    else
    {
        yuiDebug () << "Can't set loadedUI to \"" << ui << "\"" << endl;
        yuiDebug () << "It is locked to: \"" << _loadedUI << "\"" << endl;
        YUI_THROW ( YUIException ( "loadedUI is locked to: \"" + _loadedUI + "\"" ) );
    }
}


void YSettings::loadedUI( string ui )
{
    loadedUI( ui, false );
}


string YSettings::loadedUI ()
{
    yuiDebug () << "loadedUI: \"" << _loadedUI << "\"" << endl;

    return _loadedUI;
}
