/*
  Copyright (c) [2016-2020] SUSE LCC

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


// Menu Button example demonstrating also shortcut resolver.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui MenuButton1.cc -o MenuButton1

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YLayoutBox.h>
#include <yui/YEvent.h>
#include <yui/YMenuButton.h>
#include <yui/YMenuItem.h>

int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    YWidgetFactory * fac = YUI::widgetFactory();

    YDialog    * dialog  = fac->createPopupDialog();
    YLayoutBox * vbox    = fac->createVBox( dialog );
    fac->createHeading( vbox, " Menu Button Example " );
    fac->createVSpacing( vbox, 1 );

    YMenuButton * menuButton = fac->createMenuButton( vbox, "Menu" );
    fac->createVSpacing( vbox, 2 );

    YMenuItem * submenu = menuButton->addMenu( "&menu1" );
    menuButton->addItem( "&menu2" );
    menuButton->addItem( "&Menu3" ); // Test uppercase letter as shortcut
    menuButton->addItem( "menu4"  ); // Even without a shortcut marker it should find a shortcut if possible

    submenu->addItem( "&submenu1" );
    submenu->addItem( "&submenu2" );
    submenu->addSeparator();
    submenu->addItem( "&submenu3" );
    submenu->addItem( "&submenu4" );

    menuButton->resolveShortcutConflicts();
    menuButton->rebuildMenuTree();

    dialog->waitForEvent();
    dialog->destroy();
}
