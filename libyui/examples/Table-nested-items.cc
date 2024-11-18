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


// Example for a Table widget with nested items.
//
// Compile with:
//
//     g++ -I/usr/include/yui -lyui Table-nested-items.cc -o Table-nested-items

#define YUILogComponent "example"
#include <yui/YUILog.h>

#include <yui/YUI.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YTable.h>
#include <yui/YAlignment.h>
#include <yui/YLabel.h>
#include <yui/YLayoutBox.h>
#include <yui/YPushButton.h>
#include <yui/YEvent.h>


using std::string;

// Widgets

YDialog		* dialog	= 0;
YTable          * table         = 0;
YPushButton	* closeButton	= 0;


void createWidgets();
YTableHeader * tableHeader();
void populateTable();
void handleEvents();


int main( int argc, char **argv )
{
    YUILog::setLogFileName( "/tmp/libyui-examples.log" );
    YUILog::enableDebugLogging();

    createWidgets();
    populateTable();
    handleEvents();

    dialog->destroy();
}


void createWidgets()
{
    yuiMilestone() << endl;

    YWidgetFactory * fac = YUI::widgetFactory();

    dialog		 = fac->createPopupDialog();
    YAlignment * minSize = fac->createMinSize( dialog, 74, 15 );
    YAlignment * mbox	 = fac->createMarginBox( minSize, 1, 0.4 );
    YLayoutBox * vbox	 = fac->createVBox( mbox );
    YAlignment * left    = fac->createLeft( vbox );
    fac->createHeading( left, "Storage Overview" );
    fac->createVSpacing( vbox, 0.2 );

    table = fac->createTable( vbox, tableHeader() );
    table->setNotify( true );
    table->setImmediateMode( true );

    fac->createVSpacing( vbox, 0.5 );
    YAlignment * right = fac->createRight( vbox );
    closeButton = fac->createPushButton( right, "&Close" );
}


YTableHeader * tableHeader()
{
    YTableHeader * header = new YTableHeader();
    header->addColumn( "Device"      );
    header->addColumn( "Size"        , YAlignEnd   );
    header->addColumn( "Type"        );
    header->addColumn( "Mount Point" );

    return header;
}


void populateTable()
{
    yuiMilestone() << endl;

    YTableItem * sda = new YTableItem( "/dev/sda", "931.5G" );
    sda->setOpen();
    new YTableItem( sda, "/dev/sda1",  "97.7G", "ntfs", "/win/boot" );
    new YTableItem( sda, "/dev/sda2", "833.9G", "ntfs", "/win/app"  );

    YTableItem * sdb = new YTableItem( "/dev/sdb", "931.5G" );
    sdb->setClosed();
    new YTableItem( sdb, "/dev/sdb1",   "2.0G", "swap" );
    new YTableItem( sdb, "/dev/sdb2",  "29.4G", "ext4", "/hd-root-leap-42"   );
    new YTableItem( sdb, "/dev/sdb3",  "29.4G", "ext4", "/hd-root-leap-15-0" );
    new YTableItem( sdb, "/dev/sdb4", "855.8G", "xfs",  "/work" );

    YTableItem * sdc = new YTableItem( "/dev/sdc", "232.9G" );
    sdc->setOpen();
    new YTableItem( sdc, "/dev/sdc1",   "2.0G", "swap", "[swap]" );
    new YTableItem( sdc, "/dev/sdc2",  "29.4G", "ext4", "/ssd-root-leap-15-1" );
    new YTableItem( sdc, "/dev/sdc3",  "29.4G", "ext4", "/" );
    new YTableItem( sdc, "/dev/sdc4", "167.2G", "ext4", "/ssd-work" );


    // Using a YItemCollection is more efficient than adding each item one by one

    YItemCollection items;
    items.push_back( sda );
    items.push_back( sdb );
    items.push_back( sdc );

    table->addItems( items );
}


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

	    if ( event->widget() == table )	   // the table will only send events with setNotify()
	    {
                yuiMilestone() << "Event from the table" << endl;
	    }
	}
    }
}
