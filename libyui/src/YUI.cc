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

  File:		YUI.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define VERBOSE_COMM 0	// VERY verbose thread communication logging

#include <stdio.h>
#include <string.h>	// strerror()
#include <unistd.h>	// pipe()
#include <fcntl.h>  	// fcntl()
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>	// getenv()

#include <stack>

#include "Libyui_config.h"      // VERSION

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUI.h"
#include "YUILoader.h"
#include "YUISymbols.h"
#include "YDialog.h"
#include "YApplication.h"
#include "YMacro.h"
#include "YButtonBox.h"
#include "YEnvVar.h"
#include "YBuiltinCaller.h"
#include "YWidgetID.h"
#include "YUIPlugin.h"


using std::string;
using std::stack;


// Environment variable to determine button order
// (set to "KDE" or "GNOME" - case insensitive)
#define ENV_BUTTON_ORDER "Y2_BUTTON_ORDER"

// Keep dialog stack before the YUITerminator
// so that it is destroyed afterwards.
// YUITerminator deletes _yui which calls YUI::~YUI
// which accesses the dialog stack to remove all dialogs
stack<YDialog *> YDialog::_dialogStack;
YUI * YUI::_ui = 0;

static bool uiDeleted = false;

extern void * start_ui_thread( void * yui );


YUI::YUI( bool withThreads )
    : _withThreads( withThreads )
    , _uiThread( 0 )
    , _builtinCaller( 0 )
    , _terminate_ui_thread( false )
    , _eventsBlocked( false )
{
    yuiMilestone() << "This is libyui " << VERSION << endl;
    yuiMilestone() << "Creating UI " << ( withThreads ? "with" : "without" ) << " threads" << endl;

    _ui = this;
}


YUI::~YUI()
{
    if ( _ui )
    {
	if ( _withThreads && _uiThread )
	{
	    yuiError() << "shutdownThreads() was never called!"     << endl;
	    yuiError() << "shutting down now - this might segfault" << endl;
	    shutdownThreads();
	}

	if ( YDialog::openDialogsCount() > 0 )
	    yuiError() << YDialog::openDialogsCount() << " open dialogs left over" << endl;

	if ( _builtinCaller )
	    delete _builtinCaller;

	YDialog::deleteAllDialogs();

	YMacro::deleteRecorder();
	YMacro::deletePlayer();

	_ui = 0;
	uiDeleted = true;
    }
}


void
YUI::uiThreadDestructor()
{
    YDialog::deleteAllDialogs();
}


YUI *
YUI::ui()
{
    ensureUICreated();
    return _ui;
}


YWidgetFactory *
YUI::widgetFactory()
{
    static YWidgetFactory * factory = 0;

    ensureUICreated();

    if ( ! factory )
	factory = ui()->createWidgetFactory();

    YUI_CHECK_PTR( factory );
    return factory;
}


YOptionalWidgetFactory *
YUI::optionalWidgetFactory()
{
    static YOptionalWidgetFactory * factory = 0;

    ensureUICreated();

    if ( ! factory )
	factory = ui()->createOptionalWidgetFactory();

    YUI_CHECK_PTR( factory );
    return factory;
}


YApplication *
YUI::app()
{
    static YApplication * app = 0;

    ensureUICreated();

    if ( ! app )
	app = ui()->createApplication();

    YUI_CHECK_PTR( app );
    return app;
}


void YUI::ensureUICreated()
{
    if ( _ui )
	return;

    if ( uiDeleted )
	YUI_THROW( YUIException( "UI already deleted" ) );

    YUILoader::loadUI();
}


void YUI::topmostConstructorHasFinished()
{
    // The ui thread must not be started before the constructor
    // of the actual user interface is finished. Otherwise there
    // is a race condition. The ui thread would go into idleLoop()
    // before the ui is really setup. For example the Qt interface
    // does a processOneEvent in the idleLoop(). This may do a
    // repaint operation on the dialog that is just under construction!
    //
    // Therefore the creation of the thread is delayed and performed in this
    // method. It must be called at the end of the constructor of the specific
    // UI (the Qt UI or the NCurses UI).

    if ( _withThreads )
    {
	if ( pipe( pipe_from_ui ) == 0 &&
	     pipe( pipe_to_ui   ) == 0   )
	{

	    // Make fd non blockable the ui thread reads from
	    long arg;
	    arg = fcntl( pipe_to_ui[0], F_GETFL );
	    if ( fcntl( pipe_to_ui[0], F_SETFL, arg | O_NONBLOCK ) < 0 )
	    {
		yuiError() << "Couldn't set O_NONBLOCK: errno: " << errno << " " << strerror( errno ) << endl;
		_withThreads = false;
		close( pipe_to_ui[0] );
		close( pipe_to_ui[1] );
		close( pipe_from_ui[0] );
		close( pipe_from_ui[1] );
	    }
	    else
	    {
#if VERBOSE_COMM
		yuiDebug() << "Inter-thread communication pipes set up" << endl;
#endif
		_terminate_ui_thread = false;
		createUIThread();
	    }
	}
	else
	{
	    yuiError() << "pipe() failed: errno: " << errno << " " << strerror( errno ) << endl;
	    exit(2);
	}
    }
    else
    {
	yuiMilestone() << "Running without threads" << endl;
    }
}


void YUI::createUIThread()
{
    pthread_attr_t attr;
    pthread_attr_init( & attr );
    int ret = pthread_create( & _uiThread, & attr, start_ui_thread, this );

    if ( ret != 0 )
	yuiError() << "pthread_create() failed: " << errno << " " << strerror( errno ) << endl;
}


void YUI::terminateUIThread()
{
    yuiDebug() << "Sending shutdown message to UI thread" << endl;

    _terminate_ui_thread = true;
    signalUIThread();
    waitForUIThread();
    pthread_join( _uiThread, 0 );

    yuiDebug() << "UI thread shut down correctly" << endl;
}


void YUI::shutdownThreads()
{
    if ( _uiThread )
    {
	terminateUIThread();
	_uiThread = 0;
	close( pipe_to_ui[0] );
	close( pipe_to_ui[1] );
	close( pipe_from_ui[0] );
	close( pipe_from_ui[1] );
    }
}


void YUI::signalUIThread()
{
    static char arbitrary = 42;
    if ( write ( pipe_to_ui[1], & arbitrary, 1 ) == -1 )
	yuiError() <<  "Writing byte to UI thread failed" << endl;

#if VERBOSE_COMM
    yuiDebug() << "Wrote byte to UI thread" << endl;
#endif
}


bool YUI::waitForUIThread()
{
    char arbitrary;
    int result;

    do {
#if VERBOSE_COMM
	yuiDebug() << "Waiting for ui thread..." << endl;
#endif
	result = read( pipe_from_ui[0], & arbitrary, 1 );
	if ( result == -1 )
	{
	    if ( errno == EINTR || errno == EAGAIN )
		continue;
	    else
		yuiError() <<  "waitForUIThread: errno: " << errno << " " << strerror( errno ) << endl;
	}
    } while ( result == 0 );

#if VERBOSE_COMM
    yuiDebug() << "Read byte from ui thread" << endl;
#endif

    // return true if we really did get a signal byte
    return result != -1;
}


void YUI::signalYCPThread()
{
    static char arbitrary;
    if ( write( pipe_from_ui[1], & arbitrary, 1 ) == -1 )
	yuiError() <<  "Writing byte to YCP thread failed" << endl;

#if VERBOSE_COMM
    yuiDebug() << "Wrote byte to YCP thread" << endl;
#endif
}


bool YUI::waitForYCPThread()
{
    char arbitrary;

    int result;
    do {
#if VERBOSE_COMM
	yuiDebug() << "Waiting for YCP thread..." << endl;
#endif
	result = read( pipe_to_ui[0], & arbitrary, 1 );
	if ( result == -1 )
	{
	    if ( errno == EINTR || errno == EAGAIN )
		continue;
	    else
		yuiError() << "waitForYCPThread: errno: " << errno << " " << strerror( errno ) << endl;
	}
    } while ( result == 0 );

#if VERBOSE_COMM
    yuiDebug() << "Read byte from YCP thread" << endl;
#endif

    // return true if we really did get a signal byte
    return result != -1;
}


void YUI::uiThreadMainLoop()
{
    while ( true )
    {
	idleLoop( pipe_to_ui[0] );

	// The pipe is non-blocking, so we have to check if we really read a
	// signal byte. Although idleLoop already does a select(), this seems to
	// be necessary.  Anyway: Why do we set the pipe to non-blocking if we
	// wait in idleLoop for it to become readable? It is needed in
	// YUIQt::idleLoop for QSocketNotifier.

	if ( ! waitForYCPThread () )
	    continue;

	if ( _terminate_ui_thread )
	{
	    uiThreadDestructor();
	    signalYCPThread();
	    yuiDebug() << "Shutting down UI main loop" << endl;
	    return;
	}

	if ( _builtinCaller )
	    _builtinCaller->call();
	else
	    yuiError() << "No builtinCaller set" << endl;

	signalYCPThread();
    }
}


void YUI::setButtonOrderFromEnvironment()
{
    YButtonOrder buttonOrder    = YButtonBox::layoutPolicy().buttonOrder;
    YButtonOrder oldButtonOrder = buttonOrder;

    YEnvVar lastEnv;

    //
    // $DESKTOP_SESSION
    //

    YEnvVar env( "DESKTOP_SESSION" );
    yuiDebug() << env << endl;

    if ( env == "kde" ||
	 env == "xfce"  )
    {
	buttonOrder = YKDEButtonOrder;
	lastEnv     = env;
    }
    else if ( env == "gnome" )
    {
	buttonOrder = YGnomeButtonOrder;
	lastEnv     = env;
    }

    //
    // $WINDOWMANAGER
    //

    env = YEnvVar( "WINDOWMANAGER" );
    yuiDebug() << env << endl;

    if ( env.contains( "gnome" ) )
    {
	buttonOrder = YGnomeButtonOrder;
	lastEnv     = env;
    }
    else if ( env.contains( "kde" ) )
    {
	buttonOrder = YKDEButtonOrder;
	lastEnv     = env;
    }


    //
    // $Y2_BUTTON_ORDER
    //

    env = YEnvVar( ENV_BUTTON_ORDER );
    yuiDebug() << env << endl;

    if ( env == "gnome" )
    {
	buttonOrder = YGnomeButtonOrder;
	lastEnv     = env;
    }
    else if ( env == "kde" )
    {
	buttonOrder = YKDEButtonOrder;
	lastEnv     = env;
    }
    else if ( ! env.value().empty() )
    {
	yuiWarning() << "Ignoring unknown value of " << env << endl;
    }


    if ( buttonOrder != oldButtonOrder )
    {
	string buttonOrderStr;

	switch ( buttonOrder )
	{
	    case YKDEButtonOrder:
		buttonOrderStr = "KDE";
		YButtonBox::setLayoutPolicy( YButtonBox::kdeLayoutPolicy() );
		break;

	    case YGnomeButtonOrder:
		buttonOrderStr = "GNOME";
		YButtonBox::setLayoutPolicy( YButtonBox::gnomeLayoutPolicy() );
		break;

	    // Intentionally omitting "default" branch so GCC can catch unhandled enums
	}

	yuiMilestone() << "Switching to " << buttonOrderStr
		       << " button order because of " << lastEnv
		       << endl;
    }
}


YWidget *
YUI::sendWidgetID( const string & id_str )
{
    yuiMilestone() << "Sending ID \"" << id_str << "\"" << endl;
    YWidget * widget = 0;

    try
    {
        YDialog * dialog = YDialog::currentDialog(); // may throw
        YStringWidgetID id( id_str );
        widget = dialog->findWidget( &id );
        widget->setKeyboardFocus();
    }
    catch ( YUINoDialogException & ex )
    {
        YUI_CAUGHT( ex );
    }
    // Cascading any YUIWidgetNotFoundException one level up

    return widget;
}




//
// ----------------------------------------------------------------------
//

void * start_ui_thread( void * yui )
{
    YUI * ui = (YUI *) yui;

#if VERBOSE_COMM
    yuiDebug() << "Starting UI thread" << endl;
#endif

    if ( ui )
	ui->uiThreadMainLoop();
    return 0;
}

// EOF
