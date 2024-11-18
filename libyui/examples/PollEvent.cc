/*
  Copyright (c) [2019-2021] SUSE LLC
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


// libyui example for pollEvent
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui PollEvent.cc -o PollEvent


#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YLabel.h>
#include <yui/YPushButton.h>
#include <yui/YEvent.h>

int main( int argc, char **argv )
{
    YDialog    * dialog       = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox * vbox         = YUI::widgetFactory()->createVBox( dialog );
    YLabel *      label       = YUI::widgetFactory()->createLabel     ( vbox, "Event #0" );
    YPushButton * doButton    = YUI::widgetFactory()->createPushButton( vbox, "&Do Something" );
    YPushButton * closeButton = YUI::widgetFactory()->createPushButton( vbox, "&Close" );
    int eventCount = 0;

    while ( true )
    {
        YEvent * event = dialog->pollEvent();

        if ( ! event )
            continue;

        if ( event->eventType() == YEvent::CancelEvent ) // window manager "close window" button
            break; // leave event loop

        if ( event->widget() == closeButton )
            break; // leave event loop
        
        if ( event->widget() == doButton )
        {
            yuiMilestone() << "DoSomething button pressed" << std::endl;
        }

        ++eventCount;
        
        char msg[80];
        sprintf( msg, "Event #%d", eventCount );
        label->setText( msg );
    }
    
    dialog->destroy();
}
