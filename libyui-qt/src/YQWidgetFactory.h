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

  File:		YQWidgetFactory.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQWidgetFactory_h
#define YQWidgetFactory_h


#include <yui/YWidgetFactory.h>

#include "YQAlignment.h"
#include "YQButtonBox.h"
#include "YQCheckBox.h"
#include "YQCheckBoxFrame.h"
#include "YQComboBox.h"
#include "YQCustomStatusItemSelector.h"
#include "YQDialog.h"
#include "YQEmpty.h"
#include "YQFrame.h"
#include "YQImage.h"
#include "YQInputField.h"
#include "YQIntField.h"
#include "YQItemSelector.h"
#include "YQLabel.h"
#include "YQLayoutBox.h"
#include "YQLogView.h"
#include "YQMenuBar.h"
#include "YQMenuButton.h"
#include "YQMultiLineEdit.h"
#include "YQMultiSelectionBox.h"
#include "YQProgressBar.h"
#include "YQPushButton.h"
#include "YQRadioButton.h"
#include "YQRadioButtonGroup.h"
#include "YQReplacePoint.h"
#include "YQRichText.h"
#include "YQSelectionBox.h"
#include "YQSpacing.h"
#include "YQSquash.h"
#include "YQTable.h"
#include "YQTimeField.h"
#include "YQTree.h"
#include "YQBusyIndicator.h"

using std::string;


/**
 * Concrete widget factory for mandatory widgets.
 **/
class YQWidgetFactory: public YWidgetFactory
{
public:
    // Note: Using covariant return types for all createSomeWidget() methods
    // (returning YQSomeWidget where the base class declares virtual methods that
    // return YSomeWidget)


    //
    // Dialogs
    //

    virtual YQDialog *		createDialog		( YDialogType dialogType, YDialogColorMode colorMode = YDialogNormalColor );

    //
    // Layout Boxes
    //

    virtual YQLayoutBox *	createLayoutBox		( YWidget * parent, YUIDimension dim );
    virtual YQButtonBox *	createButtonBox		( YWidget * parent );

    //
    // Common Leaf Widgets
    //

    virtual YQPushButton *	createPushButton	( YWidget * parent, const std::string & label );
    virtual YQLabel *		createLabel		( YWidget * parent, const std::string & text, bool isHeading = false, bool isOutputField = false );
    virtual YQInputField *	createInputField	( YWidget * parent, const std::string & label, bool passwordMode = false );
    virtual YQCheckBox *	createCheckBox		( YWidget * parent, const std::string & label, bool isChecked = false );
    virtual YQRadioButton *	createRadioButton	( YWidget * parent, const std::string & label, bool isChecked = false );
    virtual YQComboBox *	createComboBox		( YWidget * parent, const std::string & label, bool editable	 = false );
    virtual YQSelectionBox *	createSelectionBox	( YWidget * parent, const std::string & label );
    virtual YQTree *		createTree		( YWidget * parent, const std::string & label, bool multiselection = false, bool recursiveselection = false );
    virtual YQTable *		createTable		( YWidget * parent, YTableHeader * header,     bool multiSelection = false );
    virtual YQProgressBar *	createProgressBar	( YWidget * parent, const std::string & label, int maxValue = 100 );
    virtual YQRichText *	createRichText		( YWidget * parent, const std::string & text = std::string(), bool plainTextMode = false );

    //
    // Less Common Leaf Widgets
    //

    virtual YQIntField *	createIntField		( YWidget * parent, const std::string & label, int minVal, int maxVal, int initialVal );
    virtual YQMenuButton *	createMenuButton	( YWidget * parent, const std::string & label );
    virtual YQMultiLineEdit *	createMultiLineEdit	( YWidget * parent, const std::string & label );
    virtual YQImage *		createImage		( YWidget * parent, const std::string & imageFileName, bool animated = false );
    virtual YQLogView *		createLogView		( YWidget * parent, const std::string & label, int visibleLines, int storedLines = 0 );
    virtual YQMultiSelectionBox *createMultiSelectionBox( YWidget * parent, const std::string & label );
    virtual YQBusyIndicator *	createBusyIndicator	( YWidget * parent, const std::string & label, int maxValue = 100 );

    virtual YPackageSelector *	createPackageSelector	( YWidget * parent, long modeFlags = 0 );
    virtual YWidget *		createPkgSpecial	( YWidget * parent, const std::string & name ); // NCurses only, will throw exception in the Qt UI

    //
    // Layout Helpers
    //

    virtual YQSpacing *		createSpacing		( YWidget * parent, YUIDimension dim, bool stretchable = false, YLayoutSize_t size = 0.0 );
    virtual YQEmpty *		createEmpty		( YWidget * parent );
    virtual YQAlignment *	createAlignment		( YWidget * parent, YAlignmentType horAlignment, YAlignmentType vertAlignment );
    virtual YQSquash *		createSquash		( YWidget * parent, bool horSquash, bool vertSquash );

    //
    // Visual Grouping
    //

    virtual YQFrame *		createFrame		( YWidget * parent, const std::string & label );
    virtual YQCheckBoxFrame *	createCheckBoxFrame	( YWidget * parent, const std::string & label, bool checked );

    //
    // Logical Grouping
    //

    virtual YQRadioButtonGroup *createRadioButtonGroup	( YWidget * parent );
    virtual YQReplacePoint *	createReplacePoint	( YWidget * parent );

    //
    // More leaf widgets (moved to the end to maintain ABI compatibility)
    //

    virtual YQItemSelector *             createItemSelector             ( YWidget * parent, bool enforceSingleSelection = true );
    virtual YQCustomStatusItemSelector * createCustomStatusItemSelector ( YWidget * parent, const YItemCustomStatusVector & customStates );
    virtual YQMenuBar *                  createMenuBar                  ( YWidget * parent );


protected:

    friend class YQUI;

    /**
     * Constructor.
     *
     * Use YUI::widgetFactory() to get the singleton for this class.
     **/
    YQWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~YQWidgetFactory();

}; // class YWidgetFactory


#endif // YQWidgetFactory_h
