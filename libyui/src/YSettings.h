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

  File:		YSettings.h

  Author:	Björn Esser <bjoern.esser@gmail.com>

/-*/

#ifndef YSettings_h
#define YSettings_h

#include <string>


/**
 * Predeclaration of YUILoader
 **/
class YUILoader;

/**
 * Settings for libyui
 *
 * This singleton object hold some presets for libyui.
 **/
class YSettings
{
    friend YUILoader;

public:
    /**
     * This can be used to set a subdir beneath PLUGINDIR or THEMEDIR,
     * where your program stores a custom plugin or theme.
     *
     * Once this is set, it can't be altered. If you do so although an
     * exception will be thrown.
     **/
    static void setProgDir( std::string directory );

    /**
     * Returns the value of your program's subdir.
     **/
    static std::string progDir();

    /**
     * This can be used to set a subdir ICONDIR,
     * where your program stores a custom icons.
     *
     * Once this is set, it can't be altered. If you do so although an
     * exception will be thrown.
     **/
    static void setIconDir( std::string directory );

    /**
     * Returns the value of your program's icons subdir.
     **/
    static std::string iconDir();

    /**
     * This can be used to set a subdir THEMEDIR,
     * where your program stores a custom icons.
     *
     * Once this is set, it can't be altered. If you do so although an
     * exception will be thrown.
     **/
    static void setThemeDir ( std::string directory );

    /**
     * Returns the value of your program's theme subdir.
     **/
    static std::string themeDir ();

    /**
     * This can be used to set a subdir LOCALEDIR,
     * where your program stores translations
     *
     * Once this is set, it can't be altered. If you do so although an
     * exception will be thrown.
     **/
    static void setLocaleDir( std::string directory );

    /**
     * Returns the value of your program's locale subdir.
     **/
    static std::string localeDir();

    /**
     * This can be used to set the loaded UI-backend.
     *
     * Once this is set, it can't be altered. If you do so although an
     * exception will be thrown.
     **/
    static void loadedUI( std::string ui );

    /**
     * Returns the value of the loaded UI-backend.
     **/
    static std::string loadedUI();


protected:

    /**
     * This can be used to set the loaded UI-backend.
     *
     * Once this is set, it can't be altered, except if you force it.
     * If you do so without force an exception will be thrown.
     **/
    static void loadedUI ( std::string ui, bool force );

private:

    static std::string _progDir;
    static std::string _iconDir;
    static std::string _themeDir;
    static std::string _localeDir;
    static std::string _loadedUI;

    YSettings();
    YSettings( const YSettings & );
    ~YSettings();
};

#endif // YSettings_h
