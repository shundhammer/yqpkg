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



// Libyui example for auto-wrapping labels.
//
// Local Variables:
//    compile-command: "g++ -I/usr/include/yui -lyui AutoWrap1.cc -o AutoWrap1"
// End:

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YAlignment.h>
#include <yui/YLabel.h>
#include <yui/YEvent.h>

#include "LoremIpsum.h"

using std::string;

// This should also fit in NCurses 80x24.
#define TEXT_LEN        986


int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    YWidgetFactory * fac = YUI::widgetFactory();
    YDialog    * dialog  = fac->createPopupDialog();
    YLayoutBox * vbox    = fac->createVBox( dialog );
    YAlignment * mbox    = fac->createMarginBox( vbox, 1, 0.5 );
    YAlignment * minSize = fac->createMinWidth( mbox, 74 );
    YLabel     * label   = fac->createOutputField( minSize, loremIpsum( TEXT_LEN ) );
    label->setAutoWrap();

    fac->createPushButton( vbox, "&OK" );

    dialog->waitForEvent();
    dialog->destroy();
}
