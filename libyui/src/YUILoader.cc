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

  File:		YUILoader.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <stdlib.h>		// getenv()
#include <unistd.h>		// isatty()a
#include <sys/stat.h>
#include <string.h>

#define YUILogComponent "ui"
#include "YUILog.h"
#include "YCommandLine.h"
#include "YUILoader.h"
#include "YUIPlugin.h"
#include "YUIException.h"
#include "YPath.h"
#include "YSettings.h"

#include "Libyui_config.h"

using std::string;


bool rest_enabled()
{
    const char *env = getenv("YUI_HTTP_PORT");
    return env && atoi(env) > 0;
}


void YUILoader::loadUI( bool withThreads )
{
    bool isGtk = false;
    const char * envDesktop    = getenv( "XDG_CURRENT_DESKTOP" )  ?: "";
    const char * envDisplay    = getenv( "DISPLAY" )              ?: "";
    const char * envPreset     = getenv( "YUI_PREFERED_BACKEND" ) ?: "";

    string wantedGUI;

    yuiMilestone() << "DISPLAY: \""              << envDisplay << "\"" << endl;
    yuiMilestone() << "XDG_CURRENT_DESKTOP: \""  << envDesktop << "\"" << endl;
    yuiMilestone() << "YUI_PREFERED_BACKEND: \"" << envPreset  << "\"" << endl;

    // Taken from: https://specifications.freedesktop.org/menu-spec/menu-spec-1.1.html#onlyshowin-registry
    isGtk = ( ( strstr( envDesktop, "Cinnamon" ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "GNOME"    ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "LXDE"     ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "MATE"     ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "Pantheon" ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "ROX"      ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "Unity"    ) != NULL ) || isGtk );
    isGtk = ( ( strstr( envDesktop, "XFCE"     ) != NULL ) || isGtk );

    if ( isGtk ) yuiMilestone() << "Detected a Gtk-based desktop environment." << endl
                                << "Prefering Gtk-UI if available and no" << endl
                                << "user-selected override is present." << endl;

    YCommandLine cmdline;

    bool wantGtk      = ( cmdline.find( "--gtk" )     != -1 );
    bool wantNcurses  = ( cmdline.find( "--ncurses" ) != -1 );
    bool wantQt       = ( cmdline.find( "--qt" )      != -1 );
    bool haveUIPreset = ( wantGtk || wantNcurses || wantQt );

    if ( !haveUIPreset )
    {
	wantGtk     = ( strcmp( envPreset, YUIPlugin_Gtk )     == 0 );
	wantNcurses = ( strcmp( envPreset, YUIPlugin_NCurses ) == 0 );
	wantQt      = ( strcmp( envPreset, YUIPlugin_Qt )      == 0 );
    }

    if ( wantGtk )     wantedGUI = YUIPlugin_Gtk;
    if ( wantNcurses ) wantedGUI = YUIPlugin_NCurses;
    if ( wantQt )      wantedGUI = YUIPlugin_Qt;

    yuiMilestone() << "User-selected UI-plugin: \"" << wantedGUI << "\"" << endl;

    bool haveGtk     = pluginExists( YUIPlugin_Gtk );
    bool haveNcurses = pluginExists( YUIPlugin_NCurses );
    bool haveQt      = pluginExists( YUIPlugin_Qt );

    // This reset is intentional, so the loader can work it's magic
    // selecting an UI-plugin as described in the documentation.
    wantedGUI="";

    // Set the UI-Plugin
    if ( ( haveGtk || haveQt ) && strcmp ( envDisplay, "" ) &&
	 ( !wantNcurses || !isatty( STDOUT_FILENO ) ) )
    {
	// Qt is default if available.
	if ( haveQt )
	    wantedGUI = YUIPlugin_Qt;

	// Do we want to use Gtk instead?
	if ( haveGtk && ( ( ( isGtk || wantGtk ) && !wantQt ) || !haveQt ) )
	    wantedGUI = YUIPlugin_Gtk;
    }

    else if ( haveNcurses && isatty( STDOUT_FILENO ) )
    {
	// We use NCurses.
	wantedGUI = YUIPlugin_NCurses;
    }

    // Load the wanted UI-plugin.
    if ( wantedGUI != "" )
    {
	yuiMilestone () << "Using UI-plugin: \"" << wantedGUI << "\""<< endl;
	YSettings::loadedUI( wantedGUI, true );

	try
	{
            // Load integration testing framework plugin, which load required UI
            // There is no support for GTK planned, so not loading rest api
            // plugin in case gtk was requested
            if ( rest_enabled() && wantedGUI != YUIPlugin_Gtk )
            {
                loadRestAPIPlugin( wantedGUI, withThreads );
            }
            else
            {
                loadPlugin( wantedGUI, withThreads );
            }

	    return;
	}

	catch ( YUIException & ex )
	{
	    YUI_CAUGHT( ex );

	    // Default to NCurses, if possible.
	    if ( wantedGUI != YUIPlugin_NCurses && haveNcurses && isatty( STDOUT_FILENO ) )
	    {
		yuiWarning () << "Defaulting to: \"" << YUIPlugin_NCurses << "\""<< endl;
		YSettings::loadedUI( YUIPlugin_NCurses, true );

		try
		{
		    loadPlugin( YUIPlugin_NCurses, withThreads );
		    return;
		}

		catch ( YUIException & ex )
		{
		    YUI_CAUGHT( ex );
		}
	    }

	    YUI_RETHROW( ex ); // what else to do here?
	}
    }

    else
    {
	YUI_THROW( YUICantLoadAnyUIException() );
    }
}


void YUILoader::loadRestAPIPlugin( const string & wantedGUI, bool withThreads )
{
    // Do not try to load if variable is not set
    yuiMilestone () << "Requested to start http server to control UI." << endl;

    if ( pluginExists( YUIPlugin_RestAPI ) )
    {
        YUIPlugin uiRestPlugin( YUIPlugin_RestAPI );
        createUIFunction_t createUI = nullptr;
        yuiMilestone () << "User-selected underlying UI-plugin: \"" << wantedGUI << "\"" << endl;

        if (wantedGUI == YUIPlugin_Qt)
        {
            YUIPlugin uiPluginQT( YUIPlugin_Qt );
            YUIPlugin uiPluginQTRest( YUIPlugin_Qt_RestAPI );

            if ( uiPluginQT.success() && uiRestPlugin.success() && uiPluginQTRest.success() )
            {
                yuiMilestone () << "Loading the http server to control the Qt UI" << endl;
                createUI = (createUIFunction_t) uiPluginQTRest.locateSymbol( "createYQHttpUI" );
            }
            else
                yuiError() << "Cannot load Qt REST API UI" << endl;
        }

        // fallback to ncurses + REST API if Qt does not work
        if (wantedGUI == YUIPlugin_NCurses || createUI == nullptr)
        {
            YUIPlugin uiPluginNC( YUIPlugin_NCurses );
            YUIPlugin uiPluginNCRest( YUIPlugin_Ncurses_RestAPI );
            if ( uiPluginNC.success() && uiRestPlugin.success() && uiPluginNCRest.success())
            {
                yuiMilestone () << "Loading the http server to control the ncurses UI" << endl;
                createUI = (createUIFunction_t) uiPluginNCRest.locateSymbol( "createYNCHttpUI" );
            }
        }

        if ( createUI )
        {
            YUI * ui = createUI( withThreads );
            // Same as in loadPlugin
            atexit(deleteUI);

            if ( ui ) return;
        }
    }
    // Throw an exception if loading of the plugin failed
    YUI_THROW ( YUIPluginException ( YUIPlugin_RestAPI ) );
}


void YUILoader::deleteUI()
{
    if ( YUI::_ui )
    {
        yuiMilestone() << "Shutting down UI" << endl;
        delete YUI::_ui;

        YUI::_ui = 0;
    }
}


void YUILoader::loadPlugin( const string & name, bool withThreads )
{
    if (rest_enabled() && (name == YUIPlugin_NCurses || name == YUIPlugin_Qt))
    {
        loadRestAPIPlugin(name, withThreads);
        return;
    }

    YUIPlugin uiPlugin( name.c_str() );

    if ( uiPlugin.success() )
    {
	createUIFunction_t createUI = (createUIFunction_t) uiPlugin.locateSymbol( "_Z8createUIb" ); // createUI(bool)

	if ( createUI )
	{
	    YUI * ui = createUI( withThreads ); // no threads

            // At this point the concrete UI will have loaded its own
            // internal plugins and registered their destructors.
            // Our destructor must get called before those get dlclose'd.
            //
            // Formerly ~YUI was called quite late, which called ~YQUI
            // and that ran code in the already unloaded Qt internal plugins.
            atexit(deleteUI);

	    if ( ui )
		return;
	}
    }

    YUI_THROW( YUIPluginException( name ) );
}


void YUILoader::loadExternalWidgetsPlugin ( const string& name, const string& plugin_name, const string& symbol )
{
  YUIPlugin uiPlugin ( plugin_name.c_str() );

  if ( uiPlugin.success() )
  {
    createEWFunction_t createEW = ( createEWFunction_t ) uiPlugin.locateSymbol ( symbol.c_str() );

    if ( createEW )
    {
      YExternalWidgets * we = createEW ( name.c_str() );

      if ( we )
        return;
    }
  }

  YUI_THROW ( YUIPluginException ( plugin_name ) );
}


void YUILoader::loadExternalWidgets ( const string& name, const string& symbol )
{
    string wantedGUI = name;
    wantedGUI.append( "-" );
    wantedGUI.append( YSettings::loadedUI() );

    bool haveExternal = pluginExists( wantedGUI );

    if ( haveExternal )
    {
	try
	{
	    loadExternalWidgetsPlugin(name, wantedGUI, symbol );
	    return;
	}
	catch ( YUIException & ex )
	{
	    YUI_CAUGHT( ex );
	    YUI_RETHROW( ex ); // what else to do here?
	}
    }

    else
    {
	YUI_THROW( YUICantLoadAnyUIException() );
    }
}


bool YUILoader::pluginExists( const string & pluginBaseName )
{
    struct stat fileinfo;
    string pluginName = PLUGIN_PREFIX;

    pluginName.append( pluginBaseName );
    pluginName.append( PLUGIN_SUFFIX );

    YPath plugin( PLUGINDIR, pluginName );
    bool success = stat( plugin.path().c_str(), &fileinfo) == 0;

    yuiDebug() << "UI plugin " << PLUGINDIR << "/" << pluginName
               << ( success ? " exists" : " does not exist" ) << endl;

    return success;
}
