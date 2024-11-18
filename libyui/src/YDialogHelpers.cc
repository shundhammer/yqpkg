/*
  Copyright (C) 2000-2012 Novell, Inc
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


/*-/

  File:		YDialogHelpers.cc

  Author:	Jiri Srain <jsrain@suse.cz>

/-*/


#include <sys/types.h>
#include <dirent.h>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YDialog.h"
#include "YEvent.h"
#include "YPushButton.h"
#include "YButtonBox.h"

#include "YUI.h"
#include "YApplication.h"
#include "YWidgetFactory.h"
#include "YOptionalWidgetFactory.h"
#include "YLayoutBox.h"
#include "YRichText.h"
#include "YAlignment.h"
#include "YUIException.h"
#include "YEventFilter.h"
#include "YWidgetID.h"
#include "YDumbTab.h"


using std::string;
using std::map;
using std::vector;


void
YDialog::showText( const string & text, bool useRichText )
{

    // set help text dialog size to 80% of topmost dialog, respectively 45x15 (default)

    unsigned int dialogWidth  = 45;
    unsigned int dialogHeight = 15;

    if ( ! _dialogStack.empty() )
    {
	YDialog * dialog = _dialogStack.top();
        dialogWidth  = (unsigned int) ( (float) dialog->preferredWidth()  * 0.8 );
        dialogHeight = (unsigned int) ( (float) dialog->preferredHeight() * 0.8 );
    }

    // limit dialog to a reasonable size
    if ( dialogWidth > 80 || dialogHeight > 25 )
    {
	dialogWidth = 80;
	dialogHeight = 25;
    }

    try
    {
	YDialog     * dialog    = YUI::widgetFactory()->createPopupDialog();
	YAlignment  * minSize   = YUI::widgetFactory()->createMinSize( dialog, dialogWidth, dialogHeight );
	YLayoutBox  * vbox      = YUI::widgetFactory()->createVBox( minSize );
	YUI::widgetFactory()->createRichText( vbox, text, ! useRichText );
	YButtonBox  * buttonBox = YUI::widgetFactory()->createButtonBox( vbox );
	YPushButton * okButton  = YUI::widgetFactory()->createPushButton( buttonBox, "&OK" );
	okButton->setRole( YOKButton );
	okButton->setDefaultButton();

	dialog->waitForEvent();
	dialog->destroy();
    }
    catch ( const YUIException & exception )
    {
	// Don't let the application die just because help couldn't be displayed.

	YUI_CAUGHT( exception );
    }
}


bool
YDialog::showHelpText( YWidget * widget )
{
    string helpText;

    while ( widget )
    {
	if ( ! widget->helpText().empty() )
	{
	    yuiDebug() << "Found help text for " << widget << endl;
	    helpText = widget->helpText();
	}

	widget = widget->parent();
    }

    yuiMilestone() << "Showing help text" << endl;
    showText( helpText, true );
    yuiMilestone() << "Help dialog closed" << endl;

    return true;
}


/**
 * This is called from the YRelNotesButtonHandler event filter (see YDialog.cc)
 * on a very low level when a button has the "release notes" button role
 * (YPushButton::isRelNotesButton()).
 *
 * Notice that the YQWizard has its own release notes dialog (see
 * QY2RelNotesDialog) that is used only in the Qt UI and only in the YQWizard
 * and only after UI.WizardCommand(ShowReleaseNotesButton, ...).
 **/
bool
YDialog::showRelNotesText()
{
    yuiMilestone() <<"Showing Release Notes" << endl;

    // set help text dialog size to 80% of topmost dialog, respectively 45x15 (default)

    unsigned int dialogWidth  = 45;
    unsigned int dialogHeight = 15;

    if ( ! _dialogStack.empty() )
    {
        YDialog * dialog = _dialogStack.top();
        dialogWidth  = (unsigned int) ( (float) dialog->preferredWidth()  * 0.8 );
        dialogHeight = (unsigned int) ( (float) dialog->preferredHeight() * 0.8 );
    }

    // limit dialog to a reasonable size
    if ( dialogWidth > 80 || dialogHeight > 25 )
    {
        dialogWidth = 80;
        dialogHeight = 25;
    }

    try
    {
	map<string,string> relnotes = YUI::application()->releaseNotes();

	vector<string> keys;

	for ( map<string,string>::const_iterator it = relnotes.begin(); it != relnotes.end(); ++it )
        {
	    keys.push_back( it->first );
	}

        YDialog     * dialog    = YUI::widgetFactory()->createPopupDialog();
        YAlignment  * minSize   = YUI::widgetFactory()->createMinSize( dialog, dialogWidth, dialogHeight );
        YLayoutBox  * vbox      = YUI::widgetFactory()->createVBox( minSize );
        YDumbTab    * rnTab     = 0;
        YRichText   * richtext  = 0;
        bool usePlainTextMode   = YUI::app()->isTextMode();

	// both Qt and NCurses do support DumbTab
        if ( relnotes.size() > 1 && YUI::optionalWidgetFactory()->hasDumbTab() )
	{
	    rnTab = YUI::optionalWidgetFactory()->createDumbTab( vbox );
	    int index = 0;

	    for ( map<string,string>::const_iterator it = relnotes.begin(); it != relnotes.end(); it++ )
	    {
		YItem * item = new YItem( (*it).first );
		item->setIndex( index++ );
		rnTab->addItem( item );
	    }

	    richtext = YUI::widgetFactory()->createRichText( rnTab,
                                                             (*(relnotes.begin())).second,
                                                             usePlainTextMode );
	}
	else
	{
	    richtext = YUI::widgetFactory()->createRichText( vbox,
                                                             relnotes.empty() ? "" : (*(relnotes.begin())).second,
                                                             usePlainTextMode );
	}

        YButtonBox  * buttonBox = YUI::widgetFactory()->createButtonBox( vbox );
        YPushButton * okButton  = YUI::widgetFactory()->createPushButton( buttonBox, "&OK" );
        okButton->setRole( YOKButton );
        okButton->setDefaultButton();

	while ( true )
        {
	    YEvent * event = dialog->waitForEvent();

            if ( event )
            {
                if ( event->eventType() == YEvent::MenuEvent && event->item())
                {
                    YItem * item = dynamic_cast<YItem *>( event->item() );
                    richtext->setValue( relnotes[ keys[ item->index() ] ] );
                }
                else if ( event->eventType() == YEvent::CancelEvent ||  // window manager "close window" button
                          event->widget() == okButton )
                {
                    break;
                }
            }
	}

        dialog->destroy();
    }
    catch ( const YUIException & exception )
    {
        // Don't let the application die just because the release notes couldn't be displayed.

        YUI_CAUGHT( exception );
    }

    return true;
}
