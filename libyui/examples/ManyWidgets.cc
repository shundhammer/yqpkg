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
/*-/
   File:       ManyWidgets.cc
   Author:     Michael Andres <ma@suse.de>

   Simple libyui example using a couple of different widgets.
/-*/

#include <sstream>

#define YUILogComponent "ManyWidget"
#include <yui/YUI.h>
#include <yui/YUILog.h>
#include <yui/YApplication.h>
#include <yui/YWidgetFactory.h>
#include <yui/YEvent.h>

#include <yui/YWidget.h>
#include <yui/YAlignment.h>
#include <yui/YButtonBox.h>
#include <yui/YCheckBox.h>
#include <yui/YCheckBoxFrame.h>
#include <yui/YComboBox.h>
#include <yui/YDialog.h>
#include <yui/YEmpty.h>
#include <yui/YFrame.h>
#include <yui/YImage.h>
#include <yui/YInputField.h>
#include <yui/YIntField.h>
#include <yui/YLabel.h>
#include <yui/YLayoutBox.h>
#include <yui/YLogView.h>
#include <yui/YMenuButton.h>
#include <yui/YMultiLineEdit.h>
#include <yui/YMultiSelectionBox.h>
#include <yui/YPackageSelector.h>
#include <yui/YProgressBar.h>
#include <yui/YPushButton.h>
#include <yui/YRadioButton.h>
#include <yui/YRadioButtonGroup.h>
#include <yui/YReplacePoint.h>
#include <yui/YRichText.h>
#include <yui/YSelectionBox.h>
#include <yui/YSpacing.h>
#include <yui/YSquash.h>
#include <yui/YTable.h>
#include <yui/YTableHeader.h>
#include <yui/YTimeField.h>
#include <yui/YTree.h>
#include <yui/YBusyIndicator.h>

using std::endl;

///////////////////////////////////////////////////////////////////
// widget factory convenience
///////////////////////////////////////////////////////////////////

#define LAYOUT_HELPER_CONVENIENCE( AL ) \
inline YWidget * at##AL ( YWidget * w ) { return YUI::widgetFactory()->create##AL( w ); }
LAYOUT_HELPER_CONVENIENCE( Left );
LAYOUT_HELPER_CONVENIENCE( Right );
LAYOUT_HELPER_CONVENIENCE( Top );
LAYOUT_HELPER_CONVENIENCE( Bottom );
LAYOUT_HELPER_CONVENIENCE( HCenter );
LAYOUT_HELPER_CONVENIENCE( VCenter );
LAYOUT_HELPER_CONVENIENCE( HVCenter );

inline YWidget * labeledFrameBox( YWidget * parent, const std::string & label, int hweight = 0, bool squashed = true )
{
  YWidget * frame	= YUI::widgetFactory()->createFrame( parent, label );
  if ( hweight )
    frame->setWeight( YD_HORIZ, hweight );
  frame			= YUI::widgetFactory()->createHVCenter( frame );
  if ( squashed )
    frame		= YUI::widgetFactory()->createHVSquash( frame );
  frame			= YUI::widgetFactory()->createVBox( frame );
  return frame;
}

inline YWidget * unsquashedLabeledFrameBox( YWidget * parent, const std::string & label, int hweight = 0 )
{ return labeledFrameBox( parent, label, hweight, false ); }


///////////////////////////////////////////////////////////////////
// helper
///////////////////////////////////////////////////////////////////

struct Str
{
  template<class _Tp>
  Str & operator<<( const _Tp & val )
  { _str << val; return *this; }

  operator std::string() const
  { return _str.str(); }

  std::ostringstream _str;
};

std::string debugEvent( YBusyIndicator * busy, YEvent * event )
{
  busy->setAlive( true );
  if ( ! event )
    return "<no event>";

  busy->setLabel( Str() << event->serial() );
  Str ev;
  ev << YEvent::toString( event->eventType() );
  switch ( event->eventType() )
  {
    case YEvent::WidgetEvent:
      ev << "\n- " << YEvent::toString( dynamic_cast<YWidgetEvent*>(event)->reason() );
      break;
    case YEvent::KeyEvent:
      ev << "\n- " << dynamic_cast<YKeyEvent*>(event)->keySymbol();
      break;
    default:
      break;
  }
  return ev;
}

///////////////////////////////////////////////////////////////////
// dialogs
///////////////////////////////////////////////////////////////////

void help()
{
  auto dialog		= YUI::widgetFactory()->createMainDialog();
  auto frame		= YUI::widgetFactory()->createFrame( dialog, "[ Help ]" );

  auto vbox		= YUI::widgetFactory()->createVBox( frame );
  auto rt		= YUI::widgetFactory()->createRichText( vbox );
  Str str;
  str << "<h1>Headline 1</h1>\n";
  str << "<h2>Headline 2</h2>\n";
  str << "<h3>Headline 3</h3>\n";
  str << "A sample list with attributed words:";
  str << "<li>'<bold>bold</bold>'</li>\n";
  str << "<li>'<em>emphasis</em>'</li>\n";
  str << "<li>'<pre>preformated</pre>'</li>\n";
  str << "<li>'<code>code</code>'</li>\n";
  str << "<li>'plain'</li>\n";
  str << "<li>'a bit longer plain text hopefully spanning multiple lines'</li>\n";
  rt->setValue( str );

  // buttonbox
  auto buttonbox	= YUI::widgetFactory()->createButtonBox( vbox );
  buttonbox->setLayoutPolicy( YButtonBox::gnomeLayoutPolicy() );
  auto quitButton	= YUI::widgetFactory()->createPushButton( buttonbox, "Quit" );

  while ( true )
  {
    YEvent * event = dialog->waitForEvent();
    if ( event )
    {
      // window manager "close window" button
      if ( event->eventType() == YEvent::CancelEvent || event->widget() == quitButton )
	break; // leave event loop
    }
  }
  dialog->destroy(); // should use RAII to assert this.
}


///////////////////////////////////////////////////////////////////
void popup()
{
  static YDialogColorMode mode = YDialogNormalColor;
  switch ( mode )
  {
    case YDialogNormalColor:	mode = YDialogInfoColor; break;
    case YDialogInfoColor:	mode = YDialogWarnColor; break;
    case YDialogWarnColor:	mode = YDialogNormalColor; break;
  }

  auto dialog		= YUI::widgetFactory()->createPopupDialog( mode );
  auto vbox		= YUI::widgetFactory()->createVBox( dialog );

  auto label		= YUI::widgetFactory()->createLabel( vbox, "Let it BEEP!" );
  YUI::widgetFactory()->createVSpacing( vbox );
  auto beepButton	= YUI::widgetFactory()->createPushButton( vbox, "Beep" );
  beepButton->setFunctionKey( 5 );

  // buttonbox
  auto buttonbox	= YUI::widgetFactory()->createButtonBox( vbox );
  buttonbox->setLayoutPolicy( YButtonBox::gnomeLayoutPolicy() );
  auto quitButton	= YUI::widgetFactory()->createPushButton( buttonbox, "Quit" );

  // get rid of unused variable warning
  (void)label;

  // event loop
  while ( true )
  {
    YEvent * event = dialog->waitForEvent();
    if ( event )
    {
      // window manager "close window" button
      if ( event->eventType() == YEvent::CancelEvent || event->widget() == quitButton )
	break; // leave event loop

      else if ( event->widget() == beepButton )
      {
	YUI::app()->beep();
      }
    }
  }
  dialog->destroy();
}


///////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
  //YUILog::enableDebugLogging( true );

  // auto f-keys assigned to Buttoms with that label
  // see YButtonBox for button roles.
  YUI::app()->setDefaultFunctionKey( "Help",  1 );
  YUI::app()->setDefaultFunctionKey( "Cancel",  9 );
  YUI::app()->setDefaultFunctionKey( "Quit", 10 );
  YUI::app()->setDefaultFunctionKey( "Ok", 10 );

  // layout dialog:
  YDialog    * dialog	= YUI::widgetFactory()->createMainDialog();
  YLayoutBox * vbox	= YUI::widgetFactory()->createVBox( dialog );

  // 1st row
  YWidget * hbox	= YUI::widgetFactory()->createHBox( vbox );
  hbox->setWeight( YD_VERT, 1 );

  YWidget * frame	= labeledFrameBox( hbox, "Label", 1 );
  {
    YUI::widgetFactory()->createHeading( atLeft(frame), "Headline" );
    YUI::widgetFactory()->createLabel( atLeft(frame), "Label" );
    YUI::widgetFactory()->createLabel( atLeft(frame), "Multi-\n-linelabel" );
  }

  frame 		= labeledFrameBox( hbox, "PushButton", 1 );
  {
    YUI::widgetFactory()->createPushButton( atLeft(frame), "Enabled" )->setNotify( true );
    YUI::widgetFactory()->createPushButton( atLeft(frame), "Disabled" )->setDisabled();
  }

  frame			= labeledFrameBox( hbox, "CheckButton", 1 );
  {
    YUI::widgetFactory()->createCheckBox( atLeft(frame), "Check0", false )->setNotify( true );
    YUI::widgetFactory()->createCheckBox( atLeft(frame), "Ckeck1", true )->setNotify( true );
    YUI::widgetFactory()->createCheckBox( atLeft(frame), "Disabled", false )->setDisabled();
  }

  frame			= labeledFrameBox( hbox, "RadioButton", 1 );
  {
    auto rbg		= YUI::widgetFactory()->createRadioButtonGroup( frame );
    frame		= YUI::widgetFactory()->createVBox( rbg );

    auto w = YUI::widgetFactory()->createRadioButton( atLeft(frame), "Radio0", false );
    w->setNotify( true );
    rbg->addRadioButton( w );
    w = YUI::widgetFactory()->createRadioButton( atLeft(frame), "Radio1", true );
    w->setNotify( true );
    rbg->addRadioButton( w );
    w = YUI::widgetFactory()->createRadioButton( atLeft(frame), "Disabled", false );
    w->setNotify( true );
    w->setDisabled();
    rbg->addRadioButton( w );
  }

  // 2nd row
  hbox			= YUI::widgetFactory()->createHBox( vbox );
  hbox->setWeight( YD_VERT, 1 );

  frame			= labeledFrameBox( hbox, "ComboBox", 1 );
  {
    auto cbox		= YUI::widgetFactory()->createComboBox( atLeft(frame), "Select:", false );
    cbox->setNotify( true );
    {
      YItemCollection items;
      items.push_back( new YItem( "Item 1" ) );
      items.push_back( new YItem( "Item 2" ) );
      items.push_back( new YItem( "Item 3" ) );
      cbox->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }
    cbox		= YUI::widgetFactory()->createComboBox( atLeft(frame), "Editable:", true );
    cbox->setNotify( true );
    {
      YItemCollection items;
      items.push_back( new YItem( "Item 1" ) );
      items.push_back( new YItem( "Item 2" ) );
      items.push_back( new YItem( "Item 3" ) );
      cbox->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }
    cbox		= YUI::widgetFactory()->createComboBox( atLeft(frame), "Disabled:", true );
    cbox->setDisabled();
    {
      YItemCollection items;
      items.push_back( new YItem( "Item 1" ) );
      items.push_back( new YItem( "Item 2" ) );
      items.push_back( new YItem( "Item 3" ) );
      cbox->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }
  }

  frame			= unsquashedLabeledFrameBox( hbox, "InputField", 1 );
  {
    auto tw = YUI::widgetFactory()->createInputField( atLeft(frame), "Public:" );
    tw->setStretchable( YD_HORIZ, true );
    tw->setValue( "longlongtext" );
    tw->setNotify( true );

    auto pw = YUI::widgetFactory()->createPasswordField( atLeft(frame), "Secret:" );
    pw->setStretchable( YD_HORIZ, true );
    pw->setValue( "text" );
    pw->setNotify( true );

    auto dw = YUI::widgetFactory()->createInputField( atLeft(frame), "Disabled:" );
    dw->setStretchable( YD_HORIZ, true );
    dw->setValue( "text" );
    dw->setDisabled();
  }

  frame			= unsquashedLabeledFrameBox( hbox, "Progress", 1 );
  std::pair<YIntField*,YProgressBar*> progressSync;
  {
    progressSync.first = YUI::widgetFactory()->createIntField( atLeft(frame), "IntField", 0, 100, 42 );
    progressSync.first->setNotify( true );
    progressSync.second = YUI::widgetFactory()->createProgressBar( atLeft(frame), "ProgressBar" );
    progressSync.second->setValue( 42 );
    progressSync.second->setNotify( true );
  }

  frame			= unsquashedLabeledFrameBox( hbox, "Event Loop", 1 );
  auto busySync		= YUI::widgetFactory()->createBusyIndicator( atLeft(frame), "-" );
  auto eventDebug	= YUI::widgetFactory()->createLabel( atLeft(frame), "<no event>" );
  eventDebug->setStretchable( YD_HORIZ, true );
  eventDebug->setStretchable( YD_VERT, true );
  YUI::widgetFactory()->createVStretch( frame );

  // 3rd row
  hbox			= YUI::widgetFactory()->createHBox( vbox );
  hbox->setWeight( YD_VERT, 1 );

  frame			= YUI::widgetFactory()->createCheckBoxFrame( hbox, "CheckFrame", false );
  {
    frame->setWeight( YD_HORIZ, 1 );
    frame		= YUI::widgetFactory()->createHVCenter( frame );
    frame		= YUI::widgetFactory()->createVBox( frame );

    auto w = YUI::widgetFactory()->createMultiLineEdit( frame, "MultiLineEdit" );
    w->setValue( "Check frame\nto enable\nediting." );
    w->setNotify( true );
  }

  frame			= YUI::widgetFactory()->createCheckBoxFrame( hbox, "List", false );
  {
    frame->setWeight( YD_HORIZ, 1 );
    frame		= YUI::widgetFactory()->createHVCenter( frame );
    frame		= YUI::widgetFactory()->createVBox( frame );
    auto cbox		= YUI::widgetFactory()->createSelectionBox( atLeft(frame), "Select:" );
    cbox->setNotify( true );
    {
      YItemCollection items;
      items.push_back( new YItem( "Item 1" ) );
      items.push_back( new YItem( "Item 12" ) );
      items.push_back( new YItem( "Item 123" ) );
      items.push_back( new YItem( "Item 1234" ) );
      items.push_back( new YItem( "Item 12345" ) );
      items.push_back( new YItem( "Item 123456" ) );
      cbox->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }
  }

  frame			= YUI::widgetFactory()->createCheckBoxFrame( hbox, "Tree", false );
  {
    frame->setWeight( YD_HORIZ, 1 );
    frame		= YUI::widgetFactory()->createHVCenter( frame );
    frame		= YUI::widgetFactory()->createVBox( frame );
    auto tree		= YUI::widgetFactory()->createTree( atLeft(frame), "Select:" );
    tree->setNotify( true );
    {
      YItemCollection items;
      auto t = new YTreeItem( "Item 1" );
      items.push_back( t );

      items.push_back( (t = new YTreeItem( "Item 12" )) );
      new YTreeItem( t, "Item 1" );
      items.push_back( (t = new YTreeItem( "Item 123" )) );
      new YTreeItem( t, "Item 1" );
      t = new YTreeItem( t, "Item 2" );
      new YTreeItem( t, "Item 1" );
      items.push_back( (t = new YTreeItem( "Item 1234" )) );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      t = new YTreeItem( t, "Item 3" );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      items.push_back( (t = new YTreeItem( "Item 12345" )) );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      new YTreeItem( t, "Item 3" );
      t = new YTreeItem( t, "Item 4" );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      new YTreeItem( t, "Item 3" );
      items.push_back( (t = new YTreeItem( "Item 123456" )) );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      new YTreeItem( t, "Item 3" );
      new YTreeItem( t, "Item 4" );
      t = new YTreeItem( t, "Item 5" );
      new YTreeItem( t, "Item 1" );
      new YTreeItem( t, "Item 2" );
      new YTreeItem( t, "Item 3" );
      new YTreeItem( t, "Item 4" );
      tree->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }
  }

  frame			= YUI::widgetFactory()->createCheckBoxFrame( hbox, "Table", false );
  {
    frame->setWeight( YD_HORIZ, 1 );
    frame		= YUI::widgetFactory()->createHVCenter( frame );
    frame		= YUI::widgetFactory()->createVBox( frame );

    auto head		= new YTableHeader;
    head->addColumn( "Right", YAlignEnd );
    head->addColumn( "Center", YAlignCenter );
    head->addColumn( "Left", YAlignBegin );
    auto table		= YUI::widgetFactory()->createTable( atLeft(frame), head );
    table->setNotify( true );

    YItemCollection items;
    items.push_back( new YTableItem( "a", "b", "c", "extra" ) );
    items.push_back( new YTableItem( "aa", "bb", "cc" ) );
    items.push_back( new YTableItem( "aaa", "bbb", "ccc" ) );
    items.push_back( new YTableItem( "aaaa", "bbbb", "cccc" ) );
    items.push_back( new YTableItem( "aaaaa", "bbbbb", "ccccc" ) );
    items.push_back( new YTableItem( "aaaaaa", "bbbbbb", "cccccc" ) );
    table->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
  }

  // buttonbox
  auto buttonbox	= YUI::widgetFactory()->createButtonBox( vbox );
  buttonbox->setLayoutPolicy( YButtonBox::gnomeLayoutPolicy() );
  auto helpButton	= YUI::widgetFactory()->createPushButton( buttonbox, "Help" );
  auto quitButton	= YUI::widgetFactory()->createPushButton( buttonbox, "Quit" );
  auto cancelButton	= YUI::widgetFactory()->createPushButton( buttonbox, "Cancel" );
  auto popupButton	= YUI::widgetFactory()->createPushButton( buttonbox, "&Popup" );
  popupButton->setFunctionKey( 5 );


  // event loop
  while ( true )
  {
    YEvent * event = dialog->waitForEvent();
    eventDebug->setLabel( debugEvent( busySync, event ) );

    if ( event )
    {
      // window manager "close window" button
      if ( event->eventType() == YEvent::CancelEvent
	|| event->widget() == cancelButton
	|| event->widget() == quitButton )
        break; // leave event loop

      if ( event->widget() == helpButton )
      {
	help();
      }
      else if ( event->widget() == popupButton )
      {
	popup();
      }
      else if ( event->widget() == progressSync.first )
      {
	progressSync.second->setValue( progressSync.first->value() );
      }
    }
  }
  dialog->destroy();
}
