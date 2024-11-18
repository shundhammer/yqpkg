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

  File:		YQUI.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQUI_h
#define YQUI_h

#include <qapplication.h>
#include <QMap>
#include <QTimer>
#include <QIcon>
#include <vector>
#include <type_traits>

#include <yui/YUI.h>
#include <yui/YSimpleEventHandler.h>
#include <yui/YCommandLine.h>

#define YQWidgetMargin	4
#define YQWidgetSpacing	4
#define YQButtonBorder	3

//! The class of a pointer expression.
// To be used in connect(foo, &pclass(foo)::mysignal, bar, &pclass(bar)::myslot);
// That checks types at compile time,
// unlike the string based SIGNAL and SLOT macros.
#define pclass(ptr) std::remove_reference<decltype(*ptr)>::type


class QCursor;
class QFrame;
class QStackedWidget;
class YEvent;
class YQOptionalWidgetFactory;
class YQWidgetFactory;
class YQApplication;
class YQUISignalReceiver;

using std::string;
using std::vector;

class YQUI: public YUI
{
    friend class YQUISignalReceiver;

public:

    /**
     * Constructors. Having second boolean topmostConstructor to be
     * called only when there is topmost constructor and not a plugin which
     * inherits from YQUI. Used by integration testing framework
     **/
    YQUI( bool withThreads, bool topmostConstructor = true );

    /**
     * Destructor.
     **/
    virtual ~YQUI();

    /**
     * Access the global Qt-UI.
     **/
    static YQUI * ui() { return _ui; }

    /**
     * Post-constructor initialization. If running with threads, this has to be
     * called in the UI thread. Any subsequent calls will do nothing.
     **/
    virtual void initUI();

protected:
    /**
     * Create the widget factory that provides all the createXY() methods for
     * standard (mandatory, i.e. non-optional) widgets.
     *
     * Reimplemented from YUI.
     **/
    virtual YWidgetFactory * createWidgetFactory();

    /**
     * Create the widget factory that provides all the createXY() methods for
     * optional ("special") widgets and the corresponding hasXYWidget()
     * methods.
     *
     * Reimplemented from YUI.
     **/
    virtual YOptionalWidgetFactory * createOptionalWidgetFactory();

    /*
     * Create the YApplication object that provides global methods.
     *
     * Reimplemented from YUI.
     **/
    virtual YApplication * createApplication();

public:

    /**
     * Return the global YApplication object as YQApplication.
     *
     * This will create the Y(Q)Application upon the first call and return a
     * pointer to the one and only (singleton) Y(Q)Application upon each
     * subsequent call.  This may throw exceptions if the Y(Q)Application
     * cannot be created.
     **/
    static YQApplication * yqApp();

    /**
     * Widget event handlers (slots) call this when an event occured that
     * should be the answer to a UserInput() / PollInput() (etc.) call.
     *
     * The UI assumes ownership of the event object that 'event' points to.
     * In particular, it takes care to delete that object.
     *
     * It is an error to pass 0 for 'event'.
     **/
    void sendEvent( YEvent * event );

    /**
     * Returns 'true' if there is any event pending for the specified widget.
     **/
    bool eventPendingFor( YWidget * widget ) const
	{ return _eventHandler.eventPendingFor( widget ); }

    /**
     * Returns the last event that isn't processed yet or 0 if there is none.
     *
     * The Qt UI keeps track of only one single (the last one) event.
     **/
    YEvent * pendingEvent() const { return _eventHandler.pendingEvent(); }

    /**
     * Return the pending event, if there is one, and mark it as "consumed".
     *
     * This returns 0 if there is no pending event.
     **/
    YEvent * consumePendingEvent() { return _eventHandler.consumePendingEvent(); }

    /**
     * Notification that a widget is being deleted.
     *
     * Reimplemented from YUI.
     **/
    virtual void deleteNotify( YWidget * widget );

    /**
     * Return 'true' if defaultsize windows should use the full screen.
     **/
    bool fullscreen() const { return _fullscreen; }

    /**
     * Return 'true' if defaultsize windows should not get window manager
     * borders / frames.
     **/
    bool noBorder() const { return _noborder; }
    /**
     * Returns 'true' if the UI had a fatal error that requires the application
     * to abort.
     **/
    bool fatalError() const { return _fatalError; }

    /**
     * Raise a fatal UI error. It will be delivered when it is safe to do so.
     * The caller should make sure it can continue for some time until the
     * error is delivered.
     **/
    void raiseFatalError() { _fatalError = true; }

    /**
     * Returns size for Opt(:defaultsize) dialogs (in one dimension).
     **/
    int defaultSize( YUIDimension dim ) const;

    /**
     * Make a screen shot in .png format and save it to 'filename'.
     * Opens a file selection box if 'filename' is empty.
     **/
    void makeScreenShot( std::string filename );

    /**
     * UI-specific runPkgSeleciton method: Start the package selection.
     * This implementation does the same as UserInput().
     *
     * Reimplemented from YUI.
     **/
    virtual YEvent * runPkgSelection( YWidget * packageSelector );

    /**
     * Toggle macro recording (activated by Ctrl-Shift-Alt-M):
     * Stop macro recording if it is in progress,
     * open a file selection box and ask for a macro file name to save to and
     * start recording if no recording has been in progress.
     **/
    void toggleRecordMacro();

    /**
     * Open file selection box and ask for a macro file to play
     * (activated by Ctrl-Shift-Alt-P)
     **/
    void askPlayMacro();

    /**
     * Open a pop-up dialog to ask the user for a widget ID and then send it
     * with sendWidgetID().
     **/
    void askSendWidgetID();

    /**
     * Block (or unblock) events. If events are blocked, any event sent
     * should be ignored until events are unblocked again.
     *
     * Reimplemented from YUI.
     **/
    virtual void blockEvents( bool block = true );

    /**
     * Returns 'true' if events are currently blocked.
     *
     * Reimplemented from YUI.
     **/
    virtual bool eventsBlocked() const;

    /**
     * Force unblocking all events, no matter how many times blockEvents() has
     * This returns 0 if there is no pending eventbeen called before.
     **/
    void forceUnblockEvents();

    /**
     * Show mouse cursor indicating busy state.
     **/
    void busyCursor();

    /**
     * Show normal mouse cursor not indicating busy status.
     **/
    void normalCursor();

    /**
     * Show mouse cursor indicating busy state if the UI is unable to respond
     * to user input for more than a predefined timeout (200 millisec).
     **/
    void timeoutBusyCursor();

    /**
     * Open file selection box and let the user save y2logs to that location.
     * (Shift-F8)
     **/
    void askSaveLogs();

    /**
     * Open dialog to configure logging.
     * (Shift-F7)
     **/
    void askConfigureLogging();

    /**
     * Initialize and set a textdomain for gettext()
     **/
    static void setTextdomain( const char * domain );

    /**
     * Returns the application name for the window title (e.g. "YaST2@hostname")
     **/
    QString applicationTitle() { return _applicationTitle; }

    /**
     * Sets the application name for the window title
     **/
    void setApplicationTitle( const QString & title ) { _applicationTitle = title; }

    /**
     * Load an icon. This tries several locations:
     *
     * - The icon theme from the current desktop
     * - The compiled-in Qt resources
     * - An external file
     *
     * If the icon does not have a filename extension, the icon theme will try
     * to append ".svg" and ".png". For the compiled-in Qt resources, there are
     * aliases specified ("foo" -> "foo.svg"), so it will also work without an
     * extension.
     *
     * For external files, a path and an extension will be necessary.
     *
     * If no icon could be loaded, this will return a null QIcon (check with
     * icon.isNull()), and a warning is logged.
     **/
    QIcon loadIcon( const string & iconName ) const;


protected:

    /**
     * Load an icon from the desktop theme.
     * Those icon names typically do not contain a leading path or a filename extension.
     **/
    QIcon loadThemeIcon( const QString & iconName ) const;

    /**
     * Load an icon from an absolute path (including filename extension).
     **/
    QIcon loadIconFromPath( const QString & iconPath ) const;

    /**
     * Load a built-in icon, i.e. one from the Qt resource system.
     *
     * Use either an alias from qt_icons.rcc (and no filename extension)
     * or a full filename with path and extension, e.g. "icons/foo.svg".
     * A leading ":/" will be added if not already there.
     **/
    QIcon loadBuiltInIcon( const QString & iconName ) const;


    /**
     * Handle command line args
     **/
    void processCommandLineArgs( int argc, char **argv );

    /**
     * Calculate size of Opt(:defaultsize) dialogs
     **/
    void calcDefaultSize();

    /**
     * Idle around until fd_ycp is readable and handle repaints.
     * This is only used when a separate ui thread is running.
     *
     * Reimplemented from YUI.
     **/
    virtual void idleLoop( int fd_ycp );

    /**
     * Destroy whatever needs to be destroyed within the UI thread.
     *
     * Reimplemented from YUI.
     **/
    virtual void uiThreadDestructor();

    /**
     * Notification that a YCP command has been received on fd_ycp
     * to leave idleLoop()
     **/
    void receivedYCPCommand();

    /**
     * Application shutdown
     **/
    bool close();


    //
    // Data members
    //

    static YQUI *	_ui;

    QMap<QString, int>	screenShotNo;

    bool 		_fullscreen;
    bool 		_noborder;
    QSize 		_defaultSize;

    bool 		_do_exit_loop;
    bool 		_received_ycp_command;
    bool 		_fatalError;

    QTimer * 		_busyCursorTimer;

    YSimpleEventHandler _eventHandler;
    int 		_blockedLevel;

    bool 		_leftHandedMouse;
    bool 		_askedForLeftHandedMouse;

    bool 		_uiInitialized;

    YQUISignalReceiver * _signalReceiver;
    QString 		_applicationTitle;

    // Qt copies the _reference_ to argc, so we need to store argc
    int _ui_argc;
};


/**
 * Helper class that acts as a Qt signal receiver for YQUI.
 * YQUI itself cannot be a QObject to avoid problems with starting up the UI
 * with multiple threads.
 **/
class YQUISignalReceiver : public QObject
{
    Q_OBJECT

public:
    YQUISignalReceiver();

public slots:

    void slotBusyCursor();
    void slotReceivedYCPCommand();
};


/**
 * Create a new UI if there is none yet or return the existing one if there is.
 *
 * This is the UI plugin's interface to the outside world, so don't change the
 * name or signature!
 **/
YUI * createUI( bool withThreads );


#endif // YQUI_h
