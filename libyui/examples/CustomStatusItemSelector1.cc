/*
  Copyright (c) [2019] SUSE LLC
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


// Example for ItemSelector with custom status values.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui ItemSelector3-custom-states.cc -o ItemSelector3-custom-states
//
// For a very simple ItemSelector example, see ItemSelector2-minimalistic.cc.

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YAlignment.h>
#include <yui/YButtonBox.h>
#include <yui/YCheckBox.h>
#include <yui/YFrame.h>
#include <yui/YIntField.h>
#include <yui/YItemCustomStatus.h>
#include <yui/YItemSelector.h>
#include <yui/YLabel.h>
#include <yui/YLayoutBox.h>
#include <yui/YPushButton.h>
#include <yui/YRadioButton.h>
#include <yui/YRadioButtonGroup.h>
#include <yui/YEvent.h>


using std::string;


// Config options set by askConfigValues()

bool notify	  = true;
int  visibleItems = 6;


// The custom status values are just integers. An enum is a good and simple way
// to use more speaking names.

enum CustomStatus
{
    MOD_DONT_INSTALL = 0,
    MOD_INSTALL,
    MOD_AUTOINSTALL
};

// "Software modules" used in this example to show automatic dependency
// handling and thus status change from the application's business logic.

YDescribedItem	* modKde	= 0;
YDescribedItem	* modXfce	= 0;
YDescribedItem	* modX11	= 0;
YDescribedItem	* modOffice	= 0;
YDescribedItem	* modServer	= 0;
YDescribedItem	* modSdk	= 0;


// Widgets

YDialog		* dialog	= 0;
YItemSelector	* selector	= 0;
YLabel		* resultField	= 0;
YPushButton	* resultButton	= 0;
YPushButton	* closeButton	= 0;


// Function prototypes

void createWidgets();
void handleEvents();
void handleItemClick( YItem * item );
void handleDependencies();
void autoSelectIfNeeded( YItem * item, bool needed );
void updateResultField();
void askConfigValues();



int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    askConfigValues();

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

    dialog		  = fac->createPopupDialog();
    YAlignment * mbox	  = fac->createMarginBox( dialog, 2, 2, 0.4, 0.4 );
    YAlignment * minWidth = fac->createMinWidth( mbox, 60 );
    YLayoutBox * vbox	  = fac->createVBox( minWidth );

    fac->createHeading( vbox, "Add-on Software" );
    fac->createVSpacing( vbox, 0.2 );

    YItemCustomStatusVector states;
    //					 iconName	       textIndicator  nextStatus
    states.push_back( YItemCustomStatus( "checkbox-off",	    "[  ]", MOD_INSTALL	     ) );
    states.push_back( YItemCustomStatus( "checkbox-on",		    "[ +]", MOD_DONT_INSTALL ) );
    states.push_back( YItemCustomStatus( "checkbox-auto-selected",  "[a+]", MOD_DONT_INSTALL ) );

    selector = fac->createCustomStatusItemSelector( vbox, states );
    YUI_CHECK_PTR( selector );

    modKde    = new YDescribedItem( "KDE Plasma",	"Full-fledged desktop"			);
    modXfce   = new YDescribedItem( "Xfce",		"Lightweight desktop"			);
    modX11    = new YDescribedItem( "X Window System",	"X11, simple window manager, xterm"	);
    modOffice = new YDescribedItem( "LibreOffice",	"Office suite"				);
    modServer = new YDescribedItem( "Server Tools",	"Web server, database, file server"	);
    modSdk    = new YDescribedItem( "SDK",		"Development tools"			);

    YItemCollection items;
    items.push_back( modKde    );
    items.push_back( modXfce   );
    items.push_back( modX11    );
    items.push_back( modOffice );
    items.push_back( modServer );
    items.push_back( modSdk    );
    selector->addItems( items ); // This is more efficient than repeatedly calling selector->addItem()

    selector->setVisibleItems( visibleItems );

    if ( notify )
	selector->setNotify();

    fac->createVSpacing( vbox, 0.4 );

    YLayoutBox * hbox2	= fac->createHBox( vbox );
    resultField		= fac->createOutputField( hbox2, "<Result>\n\n\n\n\n" );
    resultField->setStretchable( YD_HORIZ, true ); // allow stretching over entire dialog width

    if ( ! notify )
	resultButton	= fac->createPushButton( hbox2, "&Result" );

    fac->createVSpacing( vbox, 0.3 );

    YAlignment * rightAlignment = fac->createRight( vbox );
    closeButton			= fac->createPushButton( rightAlignment, "&Close" );
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
	    if ( event->widget()    == closeButton ||
		 event->eventType() == YEvent::CancelEvent ) // window manager "close window" button
	    {
		break; // leave event loop
	    }

	    if ( event->widget() == resultButton )
	    {
		handleDependencies();
		updateResultField();
	    }

	    if ( event->widget() == selector )
	    {
		// No YWidgetEvents from the ItemSelector in this mode (see below)
	    }

	    if ( event->eventType() == YEvent::MenuEvent )
	    {
		// In this mode, the ItemSelector does not send YWidgetEvents,
		// but YMenuEvents since (unlike YWidgetEvents) they can
		// return the item that was changed.
		//
		// Still, this requires setNotify() (the notify option to be set).

		YMenuEvent * menuEvent = dynamic_cast<YMenuEvent *>( event );

		if ( menuEvent )
		    handleItemClick( menuEvent->item() );

		updateResultField();
	    }
	}
    }
}


/**
 * Handler for user interaction (click or keyboard) with the ItemSelector's items.
 **/
void handleItemClick( YItem * item )
{
    if ( ! item )	// This might be a legitimate YMenuEvent without an item
	return;

    yuiMilestone() << "Item \"" << item->label() << "\"" << endl;
    handleDependencies();
}


/**
 * Example business logic: Change some status values based on other status values.
 *
 * If 'notify' is set, this is done automatically on each mouse click /
 * keyboard selection in the selector. Otherwise, the user needs to use the
 * "Result" button.
 **/
void handleDependencies()
{
    bool needX11    = false;
    bool needOffice = false;

    if ( modKde->selected() ) // YItem::selected() means YItem::status() != 0
    {
	needX11	   = true;
	needOffice = true;
    }

    if ( modXfce->selected() )
	needX11 = true;

    if ( modOffice->status() == MOD_INSTALL ) // not if MOD_AUTOINSTALL!
	needX11 = true;

    autoSelectIfNeeded( modOffice, needOffice );
    autoSelectIfNeeded( modX11,	   needX11   );
}


/**
 * Example business logic: Set or reset "autoinstall" status of 'item'
 * depending on the 'needed' flag.
 *
 * Notice that it is required to use YItemSelector::setItemStatus(), not just
 * YItem::setStatus(): The widget needs to be notified that the item's status
 * changed so it can update the status icon in the UI.
 **/
void autoSelectIfNeeded( YItem * item, bool needed )
{
    if ( needed && item->status() == MOD_DONT_INSTALL )
	selector->setItemStatus( item, MOD_AUTOINSTALL );

    if ( ! needed && item->status() == MOD_AUTOINSTALL )
	selector->setItemStatus( item, MOD_DONT_INSTALL );
}


/**
 * Show the selected items in the results field.
 **/
void updateResultField()
{
    selector->dumpItems();
    string result = "";

    // Iterate over the selected items, i.e. the items with status() != 0.
    // We might also simply iterate over all items if that is desired.

    for ( YItem * item: selector->selectedItems() )
    {
	string action;

	switch ( item->status() )
	{
	    case MOD_INSTALL:	  action = "Install";	   break;
	    case MOD_AUTOINSTALL: action = "Auto-install"; break;
	    default: continue;	// Skip everything else completely
	}

	if ( ! result.empty() )
	    result += "\n";

	result += action + " module " + item->label();
    }

    resultField->setText( result );
}



/**
 * Ask the user some configuration values for the main dialog.
 * All values are stored in global variables.
 *
 *    +---------------------------+
 *    |	  Program Configuration	  |
 *    |				  |
 *    |	 [x] Notify		  |
 *    |				  |
 *    |	 Visible Items:		  |
 *    |	 [ 6	      ]		  |
 *    |				  |
 *    |	  [Continue]  [Cancel]	  |
 *    +---------------------------+
 **/
void askConfigValues()
{
    yuiMilestone() << endl;

    YWidgetFactory * fac = YUI::widgetFactory();

    YDialog    * dialog = fac->createPopupDialog();
    YAlignment * mbox	= fac->createMarginBox( dialog, 2, 2, 0.4, 0.4 );
    YLayoutBox * vbox	= fac->createVBox( mbox );

    fac->createHeading( vbox, "Example Configuration" );
    fac->createVSpacing( vbox, 0.6 );

    YAlignment * left		= fac->createLeft( vbox );
    YCheckBox  * notifyCheckBox = fac->createCheckBox( left, "&Notify", notify );

    fac->createVSpacing( vbox, 0.6 );
    left = fac->createLeft( vbox );
    YIntField * visibleItemsField = fac->createIntField( left, "&Visible Items:", 1, 10, visibleItems );

    fac->createVSpacing( vbox, 1 );

    YButtonBox	* buttonBox	 = fac->createButtonBox( vbox );
    YPushButton * continueButton = fac->createPushButton( buttonBox, "C&ontinue" );
    YPushButton * cancelButton	 = fac->createPushButton( buttonBox, "&Cancel" );

    continueButton->setRole( YOKButton );
    continueButton->setDefaultButton();
    cancelButton->setRole( YCancelButton );

    YEvent * event = dialog->waitForEvent();

    if ( event->widget() == continueButton )
    {
	notify		= notifyCheckBox->value();
	visibleItems	= visibleItemsField->value();

	yuiMilestone() << "Notify: " << notify << endl;
	yuiMilestone() << "Visible items: " << visibleItems << endl;
    }
    else if ( event->widget() == cancelButton ||
	      event->eventType() == YEvent::CancelEvent ) // window manager "close window" button
    {
	dialog->destroy();
	exit( 1 );
    }

    dialog->destroy();
}
