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

  File:         YUIQt_builtins.cc

  Author:       Stefan Hundhammer <shundhammer@suse.de>

  Textdomain    "qt"

/-*/

#define USE_QT_CURSORS          1
#define FORCE_UNICODE_FONT      0

#include <sys/stat.h>
#include <unistd.h>

#include <QCursor>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QInputDialog>
#include <QWindow>
#include <QScreen>
#include <qdir.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "YQUI.h"
#include <yui/YEvent.h>
#include <yui/YMacro.h>
#include <yui/YUISymbols.h>
#include "YQDialog.h"
#include "YQSignalBlocker.h"
#include "YQApplication.h"

#include "utf8.h"
#include "YQi18n.h"


#define DEFAULT_MACRO_FILE_NAME         "macro.ycp"

using std::string;
using std::endl;



YEvent * YQUI::runPkgSelection( YWidget * packageSelector )
{
    YUI_CHECK_PTR( packageSelector );
    YEvent * event = 0;

    try
    {
	event = packageSelector->findDialog()->waitForEvent();
    }
    catch ( YUIException & uiEx )
    {
	YUI_CAUGHT( uiEx );
    }
    catch ( std::exception & e)
    {
	yuiError() << "Caught std::exception: " << e.what() << "\n"
		   << "This is a libzypp problem. Do not file a bug against the UI!"
		   << endl;
    }
    catch (...)
    {
	yuiError() << "Caught unspecified exception.\n"
		   << "This is a libzypp problem. Do not file a bug against the UI!"
		   << endl;
    }

    return event;
}


void YQUI::makeScreenShot( string stl_filename )
{
    //
    // Grab the pixels off the screen
    //

    QWidget * dialog = (QWidget *) YDialog::currentDialog()->widgetRep();
    YUI_CHECK_PTR( dialog );
    QWidget * topLevelWidget = dialog->window();
    YUI_CHECK_PTR( topLevelWidget );
    QWindow * window = topLevelWidget->windowHandle();
    YUI_CHECK_PTR( window );
    QPixmap screenShot = window->screen()->grabWindow( window->winId() );
    QString fileName ( stl_filename.c_str() );
    bool interactive = false;

    if ( fileName.isEmpty() )
    {
	interactive = true;

	// Open a file selection box. Figure out a reasonable default
	// directory / file name.

	//
	// Initialize screen shot directory
	//

	QString home = QDir::homePath();
	char * ssdir = getenv( "Y2SCREENSHOTS" );
	QString dir  = ssdir ? fromUTF8( ssdir ) : "yast2-screen-shots";

	if ( home == "/" )
	{
	    // Special case: $HOME is not set. This is normal in the inst-sys.
	    // In this case, rather than simply dumping all screen shots into
	    // /tmp which is world-writable, let's try to create a subdirectory
	    // below /tmp with restrictive permissions.
	    // If that fails, trust nobody - in particular, do not suggest /tmp
	    // as the default in the file selection box.

	    dir = "/tmp/" + dir;

	    if ( mkdir( toUTF8( dir ).c_str(), 0700 ) == -1 )
		dir = "";
	}
	else
	{
	    // For all others let's create a directory ~/yast2-screen-shots and
	    // simply ignore if this is already present. This gives the user a
	    // chance to create symlinks to a better location if he wishes so.

	    dir = home + "/" + dir;
	    (void) mkdir( toUTF8( dir ).c_str(), 0750 );
	}


	//
	// Figure out a file name
	//

	const char * baseName = "yast2";

	int no = screenShotNo[ baseName ];
	fileName = QString( "%1/%2-%3.png" )
            .arg( dir )         // %1
	    .arg( baseName )    // %2
	    .arg( no,           // %3
                  3,                    // fieldWidth (positive aligns right)
                  10,                   // base
                  QChar( '0' ) );       // fillChar

	yuiDebug() << "Screenshot: " << fileName << endl;

        fileName = YQApplication::askForSaveFileName( fileName,
                                                      QString( "*.png" ) ,
                                                      _( "Save screen shot to..." ) );

	if ( fileName.isEmpty() )
	{
	    yuiDebug() << "Save screen shot canceled by user" << endl;
	    return;
	}

	screenShotNo.insert( baseName, ++no );
    } // if fileName.isEmpty()


    //
    // Actually save the screen shot
    //

    yuiDebug() << "Saving screen shot to " << fileName << endl;
    bool success = screenShot.save( fileName, "PNG" );

    if ( ! success )
    {
	yuiError() << "Couldn't save screen shot " << fileName << endl;

	if ( interactive )
	{
	    QMessageBox::warning( YQDialog::popupParent(),
				  "Error",				// caption
				  QString( "Couldn't save screen shot\nto %1" ).arg( fileName ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  Qt::NoButton,				// button1
				  Qt::NoButton );			// button2
	}
    }
}


void YQUI::askSaveLogs()
{
    QString fileName = YQApplication::askForSaveFileName( QString( "/tmp/y2logs.tgz" ),	 	// startWith
							  QString( "*.tgz *.tar.gz"  ),		// filter
							  QString( "Save y2logs to..."  ) );	// headline

    if ( ! fileName.isEmpty() )
    {
	QString saveLogsCommand = "/usr/sbin/save_y2logs";

	if ( access( saveLogsCommand.toLatin1(), X_OK ) == 0 )
	{
	    saveLogsCommand += " '" + fileName + "'";
	    yuiMilestone() << "Saving y2logs: " << saveLogsCommand << endl;
	    int result = system( qPrintable( saveLogsCommand ) );

	    if ( result != 0 )
	    {
		yuiError() << "Error saving y2logs: \"" << saveLogsCommand
			   << "\" exited with " << result
			   << endl;

		QMessageBox::warning( YQDialog::popupParent(),
				      "Error",					// caption
				      QString( "Couldn't save y2logs to %1 - "
					       "exit code %2" ).arg( fileName ).arg( result ),
				      QMessageBox::Ok | QMessageBox::Default,	// button0
				      QMessageBox::NoButton,			// button1
				      QMessageBox::NoButton );			// button2
	    }
	    else
	    {
		yuiMilestone() << "y2logs saved to " << fileName << endl;
	    }
	}
	else
	{
	    yuiError() << "Error saving y2logs: Command \""
		       << saveLogsCommand << "\" not found"
		       << endl;

	    QMessageBox::warning( YQDialog::popupParent(),
				  "Error",					// caption
				  QString( "Couldn't save y2logs to %1:\n"
					   "Command %2 not found" ).arg( fileName ).arg( saveLogsCommand ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }
}


void YQUI::askConfigureLogging()
{
    bool okButtonPressed = false;
    QStringList items;
    items << "Debug logging off"
	  << "Debug logging on";

    QString result = QInputDialog::getItem( YQDialog::popupParent(),
                                            _("YaST Logging"),
                                            _("Configure YaST Logging:"),
                                            items, 0,
					    YUILog::debugLoggingEnabled() ? 1 : 0,
					    &okButtonPressed );
    if ( okButtonPressed )
    {
	YUILog::enableDebugLogging( result.endsWith( "on" ) );
	yuiMilestone() << "Changing logging: " << result << endl;
    }
}


void YQUI::toggleRecordMacro()
{
    if ( YMacro::recording() )
    {
	YMacro::endRecording();
        normalCursor();

        QMessageBox::information( YQDialog::popupParent(),
                                  "YaST2 Macro Recorder",                       // caption
                                  "Macro recording done.",                      // text
                                  QMessageBox::Ok | QMessageBox::Default,       // button0
                                  QMessageBox::NoButton,                        // button1
                                  QMessageBox::NoButton );                      // button2
    }
    else
    {
        normalCursor();

        QString filename =
            QFileDialog::getSaveFileName( YQDialog::popupParent(),
                                          "Select Macro File to Record to",
                                          DEFAULT_MACRO_FILE_NAME,              // startWith
                                          "*.ycp",                             // filter
                                          0,                                   // selectedFilter
                                          QFileDialog::DontUseNativeDialog
                                          );

        if ( ! filename.isEmpty() )     // file selection dialog has been cancelled
        {
            YMacro::record( toUTF8( filename ) );
        }
    }
}


void YQUI::askPlayMacro()
{
    normalCursor();

    QString filename =
        QFileDialog::getOpenFileName( YQDialog::popupParent(),
                                      "Select Macro File to Play",
                                      DEFAULT_MACRO_FILE_NAME,          // startWith
                                      "*.ycp", 0, QFileDialog::DontUseNativeDialog );
    busyCursor();

    if ( ! filename.isEmpty() ) // file selection dialog has been cancelled
    {
        YMacro::play( toUTF8( filename ) );

        // Do special magic to get out of any UserInput() loop right now
        // without doing any harm - otherwise this would hang until the next
        // mouse click on a PushButton etc.

        sendEvent( new YEvent() );
    }
}
