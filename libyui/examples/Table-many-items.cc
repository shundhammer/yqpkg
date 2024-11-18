/*
  Copyright (c) 2000 - 2012 Novell, Inc.

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


// Performance stress test for Table: Add many items
//
// Replacing items, notify and immediate modes.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui Table-many-items.cc -o Table-many-items


#include <time.h>

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YTable.h>
#include <yui/YCheckBox.h>
#include <yui/YLabel.h>
#include <yui/YPushButton.h>
#include <yui/YAlignment.h>
#include <yui/YEvent.h>
#include <stdio.h>

#define ITEM_COUNT	1000

YItemCollection pizzaItems()
{
    YItemCollection items;
    yuiMilestone() << "Creating item collection" << std::endl;

    for ( int i=1; i<= ITEM_COUNT; i++ )
    {
	char no[10];
	sprintf( no, "%04d", i );

	char name[80];
	sprintf( name, "Pizza #%05d", i );
	items.push_back( new YTableItem( no, name ) );
    }

    yuiMilestone() << "Item collection created" << std::endl;

    return items;
}


YItemCollection pastaItems()
{
    YItemCollection items;
    items.push_back( new YTableItem( "1", "Spaghetti Napoli" 	 ) );
    items.push_back( new YTableItem( "2", "Spaghetti Bolognese"  ) );
    items.push_back( new YTableItem( "3", "Fusili Emiliana" 	 ) );
    items.push_back( new YTableItem( "4", "Penne Calabrese" 	 ) );
    items.push_back( new YTableItem( "5", "Penne Al Forno" 	 ) );
    items.push_back( new YTableItem( "6", "Lasagne"	 	 ) );

    return items;
}



int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    //
    // Create and open dialog
    //

    YDialog    * dialog  = YUI::widgetFactory()->createPopupDialog();
    YAlignment * mbox    = YUI::widgetFactory()->createMarginBox( dialog, 1, 0.4 );
    YLayoutBox * vbox    = YUI::widgetFactory()->createVBox( mbox );

    // Specify larger size for the Table: It can scroll, so its size
    // depends on other widgets in the layout.
    YAlignment * minSize = YUI::widgetFactory()->createMinSize( vbox, 40, 8 ); // minWidth, minHeight


    // The Table. This is what this example all about.
    // The other widgets are just here to allow some experimenting.

    YTableHeader * header = new YTableHeader();
    YUI_CHECK_NEW( header );
    header->addColumn( "No", YAlignEnd );
    header->addColumn( "Dish" 	);

    YTable * table = YUI::widgetFactory()->createTable( minSize, header );
    table->addItems( pastaItems() );


    // Buttons to allow changing the Table items

    YLayoutBox  * buttonBox      = YUI::widgetFactory()->createHBox( vbox );
    YPushButton * pastaButton    = YUI::widgetFactory()->createPushButton( buttonBox, "&Pasta" );
    YPushButton * pizzaButton    = YUI::widgetFactory()->createPushButton( buttonBox, "Pi&zza" );
    YPushButton * clearButton    = YUI::widgetFactory()->createPushButton( buttonBox, "C&lear" );
    YPushButton * deselectButton = YUI::widgetFactory()->createPushButton( buttonBox, "&Deselect" );

    YUI::widgetFactory()->createVSpacing( vbox, 0.5 );


    // Check boxes to allow toogling "notify" and "immediate" for the Table

    YAlignment * leftAlignment = YUI::widgetFactory()->createLeft( vbox );
    YCheckBox * notifyCheckBox = YUI::widgetFactory()->createCheckBox( leftAlignment, "&Notify Mode" );
    notifyCheckBox->setNotify();

    leftAlignment = YUI::widgetFactory()->createLeft( vbox );
    YCheckBox * immediateCheckBox = YUI::widgetFactory()->createCheckBox( leftAlignment, "&Immediate Mode" );
    immediateCheckBox->setNotify();

    YUI::widgetFactory()->createVSpacing( vbox, 0.3 );


    // OutputField + button to allow monitoring the Table value
    // (and when it changes, i.e. when events are sent)

    YLayoutBox * hbox = YUI::widgetFactory()->createHBox( vbox );
    YLabel * valueField  = YUI::widgetFactory()->createOutputField( hbox, "???" );
    valueField->setStretchable( YD_HORIZ, true ); // allow stretching over entire dialog width

    YPushButton * valueButton = YUI::widgetFactory()->createPushButton( hbox, "&Value" );

    YUI::widgetFactory()->createVSpacing( vbox, 0.7 );


    // "Close" button

    YAlignment    * rightAlignment = YUI::widgetFactory()->createRight( vbox );
    YPushButton   * closeButton    = YUI::widgetFactory()->createPushButton( rightAlignment, "&Close" );


    //
    // Event loop
    //

    while ( true )
    {
	YEvent * event = dialog->waitForEvent();

	if ( event )
	{
	    if ( event->eventType() == YEvent::CancelEvent ) // window manager "close window" button
		break; // leave event loop

	    valueField->setValue( "???" );

	    if ( event->widget() == closeButton )
		break; // leave event loop
	    else if ( event->widget() == pastaButton )
	    {
		table->deleteAllItems();
		table->addItems( pastaItems() );
	    }
	    else if ( event->widget() == pizzaButton )
	    {
		table->deleteAllItems();
		YItemCollection items = pizzaItems();

		yuiMilestone() << "Adding pizza items..." << std::endl;
		time_t startTime = time(0);
		table->addItems( items );
		time_t elapsed = time(0) - startTime;

		yuiMilestone() << "Pizza items added; elapsed time: " << elapsed << " sec" << std::endl;
	    }
	    else if ( event->widget() == clearButton )
	    {
		table->deleteAllItems();
	    }
	    else if ( event->widget() == deselectButton )
	    {
		table->deselectAllItems();
	    }
	    else if ( event->widget() == notifyCheckBox )
	    {
		bool notify = notifyCheckBox->isChecked();

		if ( ! notify )
		{
		    // immediateMode implicitly includes notify, so set
		    // immediateMode off if the user wants to set notify off

		    table->setImmediateMode( false );
		    immediateCheckBox->setChecked( false );
		}

		table->setNotify( notify );
	    }
	    else if ( event->widget() == immediateCheckBox )
	    {
		bool immediate = immediateCheckBox->isChecked();
		table->setImmediateMode( immediate );

		// immediateMode implicitly includes notify;
		// reflect this in the notify check box

		if ( immediate )
		    notifyCheckBox->setChecked( true );
	    }
	    else if ( event->widget() == table ||
		      event->widget() == valueButton )
	    {
		yuiMilestone() << "Event widget: " << event->widget() << std::endl;

		YItem * item = table->selectedItem();

		if ( item )
		{
		    YTableItem * tableItem = dynamic_cast<YTableItem *> (item);

		    if ( tableItem )
			valueField->setValue( tableItem->label(1) );
		}
		else
		    valueField->setValue( "<none>" );
	    }
	    else
	    {
		yuiMilestone() << "Unknown event" << std::endl;
	    }
	}
    }


    //
    // Clean up
    //

    dialog->destroy();
}
