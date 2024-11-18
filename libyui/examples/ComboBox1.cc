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


// Simple ComboBox example.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui ComboBox1.cc -o ComboBox1
// or
//     g++ -I/usr/include/yui -lyui -DEDITABLE ComboBox1.cc -o ComboBox1
//
// for an editable ComboBox.

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YComboBox.h>
#include <yui/YLabel.h>
#include <yui/YPushButton.h>
#include <yui/YAlignment.h>
#include <yui/YEvent.h>


#ifdef EDITABLE
    bool editable = true;
#else
    bool editable = false;
#endif





int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    //
    // Create and open dialog
    //

    YDialog    * dialog  = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox * vbox    = YUI::widgetFactory()->createVBox( dialog );

    YComboBox * comboBox = YUI::widgetFactory()->createComboBox( vbox, "&Menu", editable );

    YItemCollection items;
    items.push_back( new YItem( "Pizza Margherita" 	 ) );
    items.push_back( new YItem( "Pizza Capricciosa"	 ) );
    items.push_back( new YItem( "Pizza Funghi"		 ) );
    items.push_back( new YItem( "Pizza Prosciutto"	 ) );
    items.push_back( new YItem( "Pizza Quattro Stagioni" ) );
    items.push_back( new YItem( "Calzone"		 ) );
    comboBox->addItems( items ); // This is more efficient than repeatedly calling comboBox->addItem()


    YLayoutBox * hbox = YUI::widgetFactory()->createHBox( vbox );
    YLabel * valueField  = YUI::widgetFactory()->createOutputField( hbox, "<ComboBox value unknown>" );
    valueField->setStretchable( YD_HORIZ, true ); // allow stretching over entire dialog width

    YPushButton * valueButton = YUI::widgetFactory()->createPushButton( hbox, "&Value" );

    YUI::widgetFactory()->createVSpacing( vbox, 0.3 );

    YAlignment  * rightAlignment = YUI::widgetFactory()->createRight( vbox );
    YPushButton * closeButton    = YUI::widgetFactory()->createPushButton( rightAlignment, "&Close" );


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

	    if ( event->widget() == valueButton ||
		 event->widget() == comboBox )		// comboBox will only send events with setNotify()
	    {
		// Get the current value of the ComboBox and display it in valueField.
		//
		// In normal applications, it should be clear whether or not a
		// ComboBox is editable, so the following if()...else should
		// not be necessary, only either the "if" branch or the "else" branch.

		if ( comboBox->editable() )
		{
		    // An editable ComboBox can have a selectedItem(), but the user can
		    // also enter any other text. YComboBox::value() will return that text or,
		    // if the user chose an item from the list, that item's
		    // label.
		    //
		    // YComboBox::setValue() OTOH will automatically select a list item
		    // if an item with that label exists.
		    //
		    // Semantically, an editable ComboBox is an InputField with some
		    // frequently used values in a drop-down list. This means that it
		    // makes a lot more sense to use strings and YComboBox::value() /
		    // YComboBox::setValue() consistently for such a ComboBox, not
		    // YComboBox::selectedItem() / YComboBox::selectItem().

		    valueField->setValue( comboBox->value() );
		}
		else // not editable
		{
		    // For non-editable ComboBox widgets, using items is preferred:
		    // YComboBox::selectedItem() / YComboBox::selectItem().
		    //
		    // While it is possible to use YComboBox::value() and
		    // YComboBox::setValue(), this is not the idea behind it.

		    YItem * item = comboBox->selectedItem();

		    if ( item )
			valueField->setValue( item->label() );
		    else
			valueField->setValue( "<none>" );
		}
	    }
	}
    }


    //
    // Clean up
    //

    dialog->destroy();
}
