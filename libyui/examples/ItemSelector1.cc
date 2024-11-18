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


// Comprehensive ItemSelector example.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui ItemSelector1.cc -o ItemSelector1
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
#include <yui/YItemSelector.h>
#include <yui/YLabel.h>
#include <yui/YLayoutBox.h>
#include <yui/YPushButton.h>
#include <yui/YRadioButton.h>
#include <yui/YRadioButtonGroup.h>
#include <yui/YEvent.h>


using std::string;

bool notify		= true;
bool singleSelection	= false;
int  visibleItems	= 4;
bool selectorEnabled	= true;


// Widgets

YDialog		* dialog	= 0;
YItemSelector	* selector	= 0;
YLabel		* valueField	= 0;
YPushButton	* valueButton	= 0;
YLabel		* resultField	= 0;
YPushButton	* resultButton	= 0;
YPushButton	* focusButton	= 0;
YPushButton	* enableButton	= 0;
YPushButton	* closeButton	= 0;


void createWidgets();
void handleEvents();
void updateValueField();
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

    dialog		= fac->createPopupDialog();
    YAlignment * mbox	= fac->createMarginBox( dialog, 2, 2, 0.4, 0.4 );
    YLayoutBox * vbox	= fac->createVBox( mbox );

    fac->createHeading( vbox, "Pizza Menu" );
    fac->createVSpacing( vbox, 0.2 );

    selector = fac->createItemSelector( vbox, singleSelection );
    YUI_CHECK_PTR( selector );

    YItemCollection items;
    items.push_back( new YDescribedItem( "Pizza Margherita",		"Very basic with just tomatoes and cheese"	) );
    items.push_back( new YDescribedItem( "Pizza Capricciosa",		"Ham and vegetables"				) );
    items.push_back( new YDescribedItem( "Pizza Funghi",		"Mushrooms"					) );
    items.push_back( new YDescribedItem( "Pizza Prosciutto",		"Ham"						) );
    items.push_back( new YDescribedItem( "Pizza Quattro Stagioni",	"Different toppings in each quarter"		) );
    items.push_back( new YDescribedItem( "Calzone",			"Folded over"					) );
    selector->addItems( items ); // This is more efficient than repeatedly calling selector->addItem()

    selector->setVisibleItems( visibleItems );

    if ( notify )
	selector->setNotify();

    fac->createVSpacing( vbox, 0.4 );

    YLayoutBox * hbox1	= fac->createHBox( vbox );
    valueField		= fac->createOutputField( hbox1, "<unknown>" );
    valueField->setStretchable( YD_HORIZ, true ); // allow stretching over entire dialog width

    valueButton		= fac->createPushButton( hbox1, "&Value" );

    fac->createVSpacing( vbox, 0.3 );

    YLayoutBox * hbox2	= fac->createHBox( vbox );
    resultField		= fac->createOutputField( hbox2, "<selected items>\n\n\n\n\n" );
    resultField->setStretchable( YD_HORIZ, true ); // allow stretching over entire dialog width

    resultButton		= fac->createPushButton( hbox2, "&Selected\nItems" );

    fac->createVSpacing( vbox, 0.3 );

    YAlignment	* alignment1	= fac->createLeft( vbox );
    YLayoutBox	* buttonBox	= fac->createHBox( alignment1 );
    focusButton			= fac->createPushButton( buttonBox, "Set &Focus" );
    enableButton		= fac->createPushButton( buttonBox, "&Enable"	 );

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

	    if ( event->widget() == focusButton )
		selector->setKeyboardFocus();

	    if ( event->widget() == enableButton )
	    {
		selectorEnabled = ! selectorEnabled;
		selector->setEnabled( selectorEnabled );

		// yuiMilestone() << "Enable: " << selectorEnabled << endl;
	    }

	    if ( event->widget() == resultButton )
		updateResultField();

	    if ( event->widget() == valueButton )
		updateValueField();

	    if ( event->widget() == selector )	   // the selector will only send events with setNotify()
	    {
		updateResultField();
		updateValueField();
	    }
	}
    }
}


void updateValueField()
{
    selector->dumpItems();
    YItem * item = selector->selectedItem();

    if ( item )
	valueField->setValue( item->label() );
    else
	valueField->setValue( "<none>" );
}


void updateResultField()
{
    string result = "";

    for ( YItem * item: selector->selectedItems() )
    {
	if ( ! result.empty() )
	    result += "\n";

	result += item->label();
    }

    resultField->setText( result );
}


/**
 * Ask the user some configuration values for the main dialog.
 * All values are stored in global variables.
 *
 *    +---------------------------+
 *    |   Program Configuration   |
 *    |                           |
 *    |  Selection Mode           |
 *    |  (x) Single Selection     |
 *    |  ( ) Multi-Selection      |
 *    |                           |
 *    |  [x] Notify               |
 *    |                           |
 *    |  Visible Items:           |
 *    |  [ 4          ]           |
 *    |                           |
 *    |   [Continue]  [Cancel]    |
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
    fac->createVSpacing( vbox, 0.2 );

    YAlignment		* left		   = fac->createLeft( vbox );
    YRadioButtonGroup	* radioButtonGroup = fac->createRadioButtonGroup( left );
    YFrame		* frame		   = fac->createFrame( radioButtonGroup, "Selection Mode" );
    YLayoutBox		* rbVBox	   = fac->createVBox( frame );

    left = fac->createLeft( rbVBox );
    YRadioButton * singleSelectionButton = fac->createRadioButton( left, "&Single Selection",  singleSelection );
    left = fac->createLeft( rbVBox );
    YRadioButton * multiSelectionButton	 = fac->createRadioButton( left, "&Multi-Selection", ! singleSelection );
    (void) multiSelectionButton;

    fac->createVSpacing( vbox, 0.6 );

    left = fac->createLeft( vbox );
    YCheckBox * notifyCheckBox = fac->createCheckBox( left, "&Notify", notify );

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
	singleSelection = singleSelectionButton->value();
	visibleItems	= visibleItemsField->value();

	yuiMilestone() << "Notify: " << notify << endl;
	yuiMilestone() << "Single selection: " << singleSelection << endl;
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
