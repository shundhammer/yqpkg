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


// Simple ItemSelector example.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui ItemSelector2-minimalistic.cc -o ItemSelector2-minimalistic

#include <ostream>

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YItemSelector.h>
#include <yui/YPushButton.h>
#include <yui/YAlignment.h>
#include <yui/YEvent.h>


#define SINGLE_SELECTION        true

using std::cout;



int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    //
    // Create and open dialog
    //

    YDialog    * dialog  = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox * vbox    = YUI::widgetFactory()->createVBox( dialog );

    YItemSelector * selector = YUI::widgetFactory()->createItemSelector( vbox, SINGLE_SELECTION );
    YUI_CHECK_PTR( selector );

    YItemCollection items;
    items.push_back( new YDescribedItem( "Pizza Margherita",            "Very basic with just tomatoes and cheese"      ) );
    items.push_back( new YDescribedItem( "Pizza Capricciosa",           "Ham and vegetables"                            ) );
    items.push_back( new YDescribedItem( "Pizza Funghi",                "Mushrooms"                                     ) );
    items.push_back( new YDescribedItem( "Pizza Prosciutto",            "Ham"                                           ) );
    items.push_back( new YDescribedItem( "Pizza Quattro Stagioni",      "Different toppings in each quarter"            ) );
    items.push_back( new YDescribedItem( "Calzone",                     "Folded over"                                   ) );
    selector->addItems( items ); // This is more efficient than repeatedly calling selector->addItem()

    YAlignment  * rightAlignment  = YUI::widgetFactory()->createRight( vbox );
    YPushButton * closeButton     = YUI::widgetFactory()->createPushButton( rightAlignment, "&Close" );

    //
    // Event loop
    //

    while ( true )
    {
	YEvent * event    = dialog->waitForEvent();

	if ( event )
	{
	    if ( event->eventType() == YEvent::CancelEvent || // window manager "close window" button
                 event->widget()    == closeButton           )
            {
		break; // leave event loop
            }
	}
    }

    // Make sure to retrieve any widget values before destroying the dialog!

    cout << "Selected:" << endl;

    for ( YItem * item: selector->selectedItems() )
        cout << "  " << item->label() << endl;

    //
    // Clean up
    //

    dialog->destroy();
}
