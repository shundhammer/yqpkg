/*
  Copyright (C) 2000-2017 Novell, Inc
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

  File:		YUILoader.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YUILoader_h
#define YUILoader_h


#include <string>

#include "YUI.h"
#include "YExternalWidgets.h"



#define YUIPlugin_Qt                    "qt"
#define YUIPlugin_NCurses               "ncurses"
#define YUIPlugin_Gtk                   "gtk"

#define YUIPlugin_RestAPI               "rest-api"
#define YUIPlugin_Ncurses_RestAPI       "ncurses-rest-api"
#define YUIPlugin_Qt_RestAPI            "qt-rest-api"

/**
 * Class to load one of the concrete UI plug-ins: Qt, NCurses, Gtk;
 * or one of the corresponding REST APIs used for automated testing.
 **/
class YUILoader
{
public:
    /**
     * Load any of the available UI-plugins by this order and criteria:
     *
     * - Qt:
     *    - if $DISPLAY is set
     *    - NCurses is user-selected and stdout is *not* a TTY
     *
     * - Gtk:
     *    - if $DISPLAY is set and Qt is not available,
     *    - a GTK-based desktop environment is detected
     *      from the environment variable XDG_CURRENT_DESKTOP
     *    - any of the above pre-conditions are met and
     *      NCurses is user-selected, but stdout is *not* a TTY
     *
     * - NCurses:
     *    - if $DISPLAY is *not* set and stdout is a TTY
     *    - Qt and Gtk are not available and stdout is a TTY
     *
     * This can be overridden by either:
     *
     * - specifing one of the switches on the
     *   command-line of the program
     *    - '--gtk',
     *    - '--ncurses', or
     *    - '--qt'
     *
     * - setting the environment variable
     *   YUI_PREFERED_BACKEND to one of
     *    - 'gtk',
     *    - 'ncurses', or
     *    - 'qt'
     *
     * If a command-line switch is given to the program, the
     * setting from the environment variable will be overridden
     * by the UI-plugin chosen with the switch.
     *
     * If the user-selected UI-plugin is not installed on the
     * system, an installed UI-plugin will be chosen by the
     * above criteria.
     **/
    static void loadUI( bool withThreads = false );

    /**
     * This will make sure the UI singleton is deleted.
     * If the UI is already destroyed, it will do nothing. If
     * there still is a UI object, it will be deleted.
     *
     * This is particularly important for the NCurses UI so that
     * the terminal settings are properly restored.
     **/
    static void deleteUI();

    /**
     * Method handles loading integration test framework and load underlying GUI
     * using hints from loadUI.
     **/
    static void loadRestAPIPlugin( const std::string & wantedGUI, bool withThreads = false );

    /**
     * Load a UI plug-in. 'name' is one of the YUIPlugin_ -defines above.
     *
     * This might throw exceptions.
     **/
    static void loadPlugin( const std::string & name, bool withThreads = false );

    /**
     * Check if a plug-in exists.
     **/
    static bool pluginExists( const std::string & pluginBaseName );

    /**
     * Load the given External Widgets plugin followed by its graphical extension implementation
     * in the following order in the same way as loadUI:
     * - Qt, Gtk or NCurses
     *
     * 'name'   is the user defined plugin name, graphical extension implementations have to
     *          be called 'name'-qt, 'name'-gtk and 'name'-ncurses. Following this rule plugin
     *          file names are as libyui-XX-YY.so.VER where:
     *               XX  is the user defined name
     *               YY  is the UI used (ncurses, gtk, qt)
     *               VER is the libyui so version
     * 'symbol' is the function symbol to be loaded, e.g. YExternalWidgets* 'symbol'(void)
     *          (e.g. default YExternalWidgets* createExternalWidgets(const char *)
     *          see createEWFunction_t definition)
     **/
    static void loadExternalWidgets( const std::string & name,
                                     const std::string & symbol = "_Z21createExternalWidgetsPKc" );

private:
    YUILoader()  {}
    ~YUILoader() {}

    /**
     * Used by loadExternalWidgets to load the graphical plugin specialization.
     *
     * 'name'        is the original plugin name (e.g. the one passed to loadExternalWidgets)
     * 'plugin_name' is the graphical plugin specialization name (e.g. 'name'-[gtk|ncurses|qt])
     * 'symbol'      is the function symbol to be loaded and executed (e.g. the one passed loadExternalWidgets)
     *
     * This might throw exceptions:
     * YUIPluginException if the plugin has not been loaded
     * specific exception can be thrown by funtion invoked (param symbol)
     **/
    static void loadExternalWidgetsPlugin( const std::string & name,
                                           const std::string & plugin_name,
                                           const std::string & symbol );
};


/**
 * Every UI plug-in has to provide a function
 *
 *     YUI * createUI( bool withThreads )
 *
 * that creates a UI of that specific type upon the first call and returns that
 * singleton for all subsequent calls.
 **/
typedef YUI * (*createUIFunction_t)( bool );

/**
 * Every WE extension plug-in has to provide a function
 *
 *     YExternalWidgets * createWE( )
 *
 * that creates a WE of that specific type upon the first call and returns that
 * singleton for all subsequent calls.
 **/
typedef YExternalWidgets * (*createEWFunction_t)( const char * );

/**
 * For integration testing, YUI has separate framework which allows to have
 * control over UI using REST API. The server has to be started after testing
 * if the framework plugin is loaded, which is done by the method which creates
 * the server instance.  To avoid having to include additional type, we define
 * it as 'void' here.
 **/
typedef void (*getServerFunction_t)();


#endif // YUILoader_h
