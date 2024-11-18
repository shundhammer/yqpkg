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
//     g++ -I/usr/include/yui -lyui MenuBar1.cc -o MenuBar1

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YAlignment.h>
#include <yui/YDialog.h>
#include <yui/YLabel.h>
#include <yui/YCheckBox.h>
#include <yui/YLayoutBox.h>
#include <yui/YMenuBar.h>
#include <yui/YSquash.h>
#include <yui/YWidgetFactory.h>
#include <yui/YShortcut.h>
#include <yui/YEvent.h>
#include <yui/YPushButton.h>


using std::string;


// Widgets

YDialog	    * dialog		= 0;
YMenuBar    * menuBar		= 0;
YLabel	    * lastEventLabel	= 0;
YCheckBox   * readOnlyCheckBox	= 0;
YCheckBox   * visibleCheckBox	= 0;
YPushButton * closeButton	= 0;


// Menus and Menu Items

YMenuItem * fileMenu		= 0;
YMenuItem * editMenu		= 0;
YMenuItem * viewMenu		= 0;
YMenuItem * optionsMenu		= 0;
YMenuItem * contentsMenu	= 0;


// "File" menu

YMenuItem * actionOpen		= 0;
YMenuItem * actionSave		= 0;
YMenuItem * actionSaveAs	= 0;
YMenuItem * actionQuit		= 0;


// "Edit" menu

YMenuItem * actionCut		= 0;
YMenuItem * actionCopy		= 0;
YMenuItem * actionPaste		= 0;


// "View" menu

YMenuItem * actionViewNormal	= 0;
YMenuItem * actionViewCompact	= 0;
YMenuItem * actionViewDetailed	= 0;


// "View" -> "Zoom" submenu

YMenuItem * zoomMenu		= 0;
YMenuItem * actionZoomIn	= 0;
YMenuItem * actionZoomOut	= 0;
YMenuItem * actionZoomDefault	= 0;


// "Options" menu

YMenuItem * actionSettings	= 0;


// "Contents" menu

YMenuItem * actionClear		= 0;


// Function Prototypes

void createWidgets();
void addMenus( YMenuBar * menuBar );
void handleEvents();
void showEvent( YMenuEvent * event );
void updateActions();




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
    YLayoutBox * vbox1	 = fac->createVBox( minSize );
    menuBar		 = fac->createMenuBar( vbox1 );

    addMenus( menuBar );

    YAlignment * center	 = fac->createHVCenter( vbox1 );
    YSquash    * squash	 = fac->createHVSquash( center );
    YLayoutBox * vbox2	 = fac->createVBox( squash );

    YAlignment * left = fac->createLeft( vbox2 );
    fac->createLabel   ( left, "Last Event:" );
    fac->createVSpacing( vbox2, 0.2 );
    YAlignment * minWidth = fac->createMinWidth( vbox2, 15 );
    lastEventLabel = fac->createOutputField( minWidth, "<none>" );

    fac->createVSpacing( vbox2, 2 );
    readOnlyCheckBox = fac->createCheckBox( vbox2, "Read &Only", true );
    readOnlyCheckBox->setNotify( true );

    fac->createVSpacing( vbox2, 0.2 );
    visibleCheckBox = fac->createCheckBox( vbox2, "&Visible", true );
    visibleCheckBox->setNotify( true );

    fac->createVSpacing( vbox2, 1 );
    closeButton = fac->createPushButton( vbox2, "&Close" );
}


void addMenus( YMenuBar * menuBar )
{
    // The difference between a menu and a plain menu item is just that the
    // menu has child items whild the plain item does not.

    fileMenu	 = menuBar->addMenu( "&File" );
    editMenu	 = menuBar->addMenu( "&Edit" );
    viewMenu	 = menuBar->addMenu( "&View" );
    optionsMenu  = menuBar->addMenu( "&Options" );
    contentsMenu = menuBar->addMenu( "&Contents" );

    actionOpen		= fileMenu->addItem( "&Open..."	   );
    actionSave		= fileMenu->addItem( "&Save"	   );
    actionSaveAs	= fileMenu->addItem( "Save &As..." );
    fileMenu->addSeparator();
    actionQuit		= fileMenu->addItem( "&Quit" );

    actionCut		= editMenu->addItem( "C&ut"   );
    actionCopy		= editMenu->addItem( "&Copy"  );
    actionPaste		= editMenu->addItem( "&Paste" );

    actionViewNormal	= viewMenu->addItem( "&Normal"	 );
    actionViewCompact	= viewMenu->addItem( "&Compact"	 );
    actionViewDetailed	= viewMenu->addItem( "&Detailed" );
    viewMenu->addSeparator();
    zoomMenu		= viewMenu->addMenu( "&Zoom" );

    actionZoomIn	= zoomMenu->addItem( "Zoom &In"	     );
    actionZoomOut	= zoomMenu->addItem( "Zoom &Out"     );
    actionZoomDefault	= zoomMenu->addItem( "Zoom &Default" );

    actionSettings	= optionsMenu->addItem( "&Settings..." );

    actionClear		= contentsMenu->addItem( "&Clear" );

    menuBar->resolveShortcutConflicts();
    menuBar->rebuildMenuTree();
}


/**
 * Event loop and event handling for the main dialog
 **/
void handleEvents()
{
    yuiMilestone() << endl;

    dialog->open();
    updateActions();

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

	    if ( event->widget() == readOnlyCheckBox )
		updateActions();

	    if ( event->widget() == visibleCheckBox )
		updateActions();

	    if ( event->widget() == closeButton )
		break;
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


/**
 * Update the available menu actions:
 *   * Enable or disable some of them according to the current status of the "Read Only" check box.
 *   * Show or hide some of them according to the current status of the "Visible" check box.
 **/
void updateActions()
{
    bool readOnly = readOnlyCheckBox->isChecked();

    menuBar->setItemEnabled( actionSave,  ! readOnly );
    menuBar->setItemEnabled( actionCut,	  ! readOnly );
    menuBar->setItemEnabled( actionPaste, ! readOnly );

    bool visible = visibleCheckBox->isChecked();

    menuBar->setItemVisible( contentsMenu, visible );
    menuBar->setItemVisible( actionOpen, visible );
}
