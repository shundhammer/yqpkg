/*
  Copyright (c) [2020] SUSE LLC
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


// MenuBar example.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui MenuBar2.cc -o MenuBar2

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YAlignment.h>
#include <yui/YDialog.h>
#include <yui/YLabel.h>
#include <yui/YLayoutBox.h>
#include <yui/YMenuBar.h>
#include <yui/YSquash.h>
#include <yui/YWidgetFactory.h>
#include <yui/YShortcut.h>
#include <yui/YEvent.h>


using std::string;


// Widgets

YDialog * dialog                = 0;
YLabel  * lastEventLabel        = 0;


// Menus and Menu Items

YMenuItem * fileMenu            = 0;
YMenuItem * editMenu            = 0;
YMenuItem * viewMenu            = 0;
YMenuItem * optionsMenu         = 0;


// "File" menu

YMenuItem * actionOpen          = 0;
YMenuItem * actionSave          = 0;
YMenuItem * actionSaveAs        = 0;
YMenuItem * actionQuit          = 0;


// "Edit" menu

YMenuItem * actionCut           = 0;
YMenuItem * actionCopy          = 0;
YMenuItem * actionPaste         = 0;


// "View" menu

YMenuItem * actionViewNormal    = 0;
YMenuItem * actionViewCompact   = 0;
YMenuItem * actionViewDetailed  = 0;


// "View" -> "Zoom" submenu

YMenuItem * zoomMenu            = 0;
YMenuItem * actionZoomIn        = 0;
YMenuItem * actionZoomOut       = 0;
YMenuItem * actionZoomDefault   = 0;

// "Options" menu

YMenuItem * actionSettings      = 0;


// Function Prototypes

void createWidgets();
void addMenus( YMenuBar * menuBar );
void handleEvents();
void showEvent( YMenuEvent * event );




int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    createWidgets();
    handleEvents();

    dialog->destroy();
}


/**
 * Create the widget tree for the main dialog
 **/
void createWidgets()
{
    yuiMilestone() << endl;

    YWidgetFactory * fac = YUI::widgetFactory();

    dialog		 = fac->createPopupDialog();
    YAlignment * minSize = fac->createMinSize( dialog, 50, 20 );
    YLayoutBox * vbox1   = fac->createVBox( minSize );
    YMenuBar   * menuBar = fac->createMenuBar( vbox1 );

    addMenus( menuBar );

    YAlignment * center  = fac->createHVCenter( vbox1 );
    YSquash    * squash  = fac->createHVSquash( center );
    YLayoutBox * vbox2   = fac->createVBox( squash );

    YAlignment * left = fac->createLeft( vbox2 );
    fac->createLabel   ( left, "Last Event:" );
    fac->createVSpacing( vbox2, 0.2 );
    YAlignment * minWidth = fac->createMinWidth( vbox2, 15 );
    lastEventLabel = fac->createOutputField( minWidth, "<none>" );
}


void addMenus( YMenuBar * menuBar )
{
    // This uses the more generic, but also more clumsy API:
    //
    // Each object in a menu is just a YMenuItem, no matter if it's a
    // menu/submenu, a plain item or a separator.
    //
    // The difference between a menu and a plain menu item is just that the
    // menu has child items whild the plain item does not. A separator is just
    // a menu item with an empty label.
    //
    // For the more elagant and more self-descriptive API, see the MenuBar1.cc
    // example.

    fileMenu    = new YMenuItem( "&File" );
    editMenu    = new YMenuItem( "&Edit" );
    viewMenu    = new YMenuItem( "&View" );
    optionsMenu = new YMenuItem( "&Options" );

    actionOpen          = new YMenuItem( fileMenu, "&Open..."    );
    actionSave          = new YMenuItem( fileMenu, "&Save"       );
    actionSaveAs        = new YMenuItem( fileMenu, "Save &As..." );

    new YMenuItem( fileMenu, "" ); // separator

    actionQuit          = new YMenuItem( fileMenu, "&Quit"       );

    actionCut           = new YMenuItem( editMenu, "C&ut"   );
    actionCopy          = new YMenuItem( editMenu, "&Copy"  );
    actionPaste         = new YMenuItem( editMenu, "&Paste" );

    actionViewNormal    = new YMenuItem( viewMenu, "&Normal"   );
    actionViewCompact   = new YMenuItem( viewMenu, "&Compact"  );
    actionViewDetailed  = new YMenuItem( viewMenu, "&Detailed" );

    new YMenuItem( viewMenu, "" ); // separator

    zoomMenu            = new YMenuItem( viewMenu, "&Zoom"         );
    actionZoomIn        = new YMenuItem( zoomMenu, "Zoom &In"      );
    actionZoomOut       = new YMenuItem( zoomMenu, "Zoom &Out"     );
    actionZoomDefault   = new YMenuItem( zoomMenu, "Zoom &Default" );

    actionSettings      = new YMenuItem( optionsMenu, "&Settings..." );

    menuBar->addItem( fileMenu );
    menuBar->addItem( editMenu );
    menuBar->addItem( viewMenu );
    menuBar->addItem( optionsMenu );

    // Do NOT add all the individual items separately to the menu bar:
    // They are owned by their parent menu.

    menuBar->resolveShortcutConflicts();
    menuBar->rebuildMenuTree();
}


/**
 * Event loop and event handling for the main dialog
 **/
void handleEvents()
{
    yuiMilestone() << endl;

    while ( true )
    {
	YEvent * event = dialog->waitForEvent();

	if ( event )
	{
	    if ( event->eventType() == YEvent::CancelEvent ) // window manager "close window" button
	    {
		break; // leave event loop
	    }

            if ( event->eventType() == YEvent::MenuEvent )
            {
                YMenuEvent * menuEvent = dynamic_cast<YMenuEvent *>( event );

                if ( menuEvent )
                {
                    showEvent( menuEvent );

                    if ( menuEvent->item() == actionQuit )
                        break; // leave event loop
                }
            }
	}
    }
}


/**
 * Show the (label of) the item causing a menu event in the 'lastEventLabel'
 * widget.
 **/
void showEvent( YMenuEvent * event )
{
    if ( event && event->item() )
    {
        string text = YShortcut::cleanShortcutString( event->item()->label() );
        lastEventLabel->setLabel( text );
    }
}
