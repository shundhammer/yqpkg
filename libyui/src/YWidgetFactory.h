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

  File:		YWidgetFactory.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YWidgetFactory_h
#define YWidgetFactory_h

#include <string>

#include "YTypes.h"
#include "YItemCustomStatus.h"


class YWidget;
class YAlignment;
class YBusyIndicator;
class YButtonBox;
class YCheckBox;
class YCheckBoxFrame;
class YComboBox;
class YDialog;
class YEmpty;
class YFrame;
class YImage;
class YInputField;
class YIntField;
class YItemSelector;
class YLabel;
class YLayoutBox;
class YLogView;
class YMenuBar;
class YMenuButton;
class YMultiLineEdit;
class YMultiSelectionBox;
class YPackageSelector;
class YProgressBar;
class YPushButton;
class YRadioButton;
class YRadioButtonGroup;
class YReplacePoint;
class YRichText;
class YSelectionBox;
class YSpacing;
class YSquash;
class YTable;
class YTableHeader;
class YTimeField;
class YTree;



/**
 * Abstract widget factory for mandatory widgets.
 * Use YOptionalWidgetFactory for optional ("special") widgets.
 *
 * Refer to the respective widget's documentation (in the header file) for
 * documentation about the function parameters.
 **/
class YWidgetFactory
{
public:

    //
    // Dialogs
    //

    YDialog *			createMainDialog	( YDialogColorMode colorMode = YDialogNormalColor );
    YDialog *			createPopupDialog	( YDialogColorMode colorMode = YDialogNormalColor );
    virtual YDialog *		createDialog		( YDialogType dialogType, YDialogColorMode colorMode = YDialogNormalColor ) = 0;

    //
    // Layout Boxes
    //

    YLayoutBox *		createVBox		( YWidget * parent );
    YLayoutBox *		createHBox		( YWidget * parent );
    virtual YLayoutBox *	createLayoutBox		( YWidget * parent, YUIDimension dimension )	= 0;
    virtual YButtonBox *	createButtonBox		( YWidget * parent )				= 0;

    //
    // Common Leaf Widgets
    //

    virtual YPushButton *	createPushButton	( YWidget * parent, const std::string & label )					= 0;
    virtual YLabel *		createLabel		( YWidget * parent, const std::string & text, bool isHeading = false, bool isOutputField = false ) = 0;
    YLabel *			createHeading		( YWidget * parent, const std::string & label );
    virtual YInputField *	createInputField	( YWidget * parent, const std::string & label, bool passwordMode = false )	= 0;
    virtual YCheckBox *		createCheckBox		( YWidget * parent, const std::string & label, bool isChecked	 = false )	= 0;
    virtual YRadioButton *	createRadioButton	( YWidget * parent, const std::string & label, bool isChecked	 = false )	= 0;
    virtual YComboBox *		createComboBox		( YWidget * parent, const std::string & label, bool editable	 = false )	= 0;
    virtual YSelectionBox *	createSelectionBox	( YWidget * parent, const std::string & label )					= 0;
    virtual YTree *		createTree		( YWidget * parent, const std::string & label, bool multiselection = false, bool recursiveselection = false ) = 0;
    virtual YTable *		createTable		( YWidget * parent, YTableHeader * header_disown, bool multiSelection = false )	= 0;
    virtual YProgressBar *	createProgressBar	( YWidget * parent, const std::string & label, int  maxValue	   = 100   )	= 0;
    virtual YRichText *		createRichText		( YWidget * parent, const std::string & text = std::string(), bool plainTextMode = false )	= 0;
    virtual YBusyIndicator *	createBusyIndicator	( YWidget * parent, const std::string & label, int timeout = 1000 )		= 0;

    //
    // Less Common Leaf Widgets
    //

    YPushButton *		createIconButton	( YWidget * parent, const std::string & iconName, const std::string & fallbackTextLabel );
    YLabel *			createOutputField	( YWidget * parent, const std::string & label );
    virtual YIntField *		createIntField		( YWidget * parent, const std::string & label, int minVal, int maxVal, int initialVal )	= 0;
    YInputField *		createPasswordField	( YWidget * parent, const std::string & label );

    virtual YMenuButton *	createMenuButton	( YWidget * parent, const std::string & label )						= 0;
    virtual YMultiLineEdit *	createMultiLineEdit	( YWidget * parent, const std::string & label )						= 0;
    virtual YImage *		createImage		( YWidget * parent, const std::string & imageFileName, bool animated = false )		= 0;
    virtual YLogView *		createLogView		( YWidget * parent, const std::string & label, int visibleLines, int storedLines = 0 )	= 0;
    virtual YMultiSelectionBox *createMultiSelectionBox ( YWidget * parent, const std::string & label )						= 0;

    virtual YPackageSelector *	createPackageSelector	( YWidget * parent, long ModeFlags = 0 )		= 0;
    virtual YWidget *		createPkgSpecial	( YWidget * parent, const std::string & subwidgetName )	= 0; // NCurses only

    //
    // Layout Helpers
    //

    YSpacing *			createHStretch		( YWidget * parent );
    YSpacing *			createVStretch		( YWidget * parent );
    YSpacing *			createHSpacing		( YWidget * parent, YLayoutSize_t size = 1.0 );
    YSpacing *			createVSpacing		( YWidget * parent, YLayoutSize_t size = 1.0 );
    virtual YSpacing *		createSpacing		( YWidget * parent, YUIDimension dim, bool stretchable = false, YLayoutSize_t size = 0.0 ) = 0;
    virtual YEmpty *		createEmpty		( YWidget * parent )	= 0;

    YAlignment *		createLeft		( YWidget * parent );
    YAlignment *		createRight		( YWidget * parent );
    YAlignment *		createTop		( YWidget * parent );
    YAlignment *		createBottom		( YWidget * parent );
    YAlignment *		createHCenter		( YWidget * parent );
    YAlignment *		createVCenter		( YWidget * parent );
    YAlignment *		createHVCenter		( YWidget * parent );

    YAlignment *		createMarginBox		( YWidget * parent, YLayoutSize_t horMargin, YLayoutSize_t vertMargin );
    YAlignment *		createMarginBox		( YWidget * parent,
							  YLayoutSize_t leftMargin, YLayoutSize_t rightMargin,
							  YLayoutSize_t topMargin,  YLayoutSize_t bottomMargin );

    YAlignment *		createMinWidth		( YWidget * parent, YLayoutSize_t minWidth  );
    YAlignment *		createMinHeight		( YWidget * parent, YLayoutSize_t minHeight );
    YAlignment *		createMinSize		( YWidget * parent, YLayoutSize_t minWidth, YLayoutSize_t minHeight );

    virtual YAlignment *	createAlignment		( YWidget * parent, YAlignmentType horAlignment, YAlignmentType vertAlignment )	= 0;

    YSquash *			createHSquash		( YWidget * parent );
    YSquash *			createVSquash		( YWidget * parent );
    YSquash *			createHVSquash		( YWidget * parent );
    virtual YSquash *		createSquash		( YWidget * parent, bool horSquash, bool vertSquash ) = 0;

    //
    // Visual Grouping
    //

    virtual YFrame *		createFrame		( YWidget * parent, const std::string & label )			= 0;
    virtual YCheckBoxFrame *	createCheckBoxFrame	( YWidget * parent, const std::string & label, bool checked )	= 0;

    //
    // Logical Grouping
    //

    virtual YRadioButtonGroup *	createRadioButtonGroup	( YWidget * parent )	= 0;
    virtual YReplacePoint *	createReplacePoint	( YWidget * parent )	= 0;

    //
    // More leaf widgets (moved to the end to maintain ABI compatibility)
    //

    virtual YItemSelector *	createItemSelector	       ( YWidget * parent, bool enforceSingleSelection = true );
    YItemSelector *		createSingleItemSelector       ( YWidget * parent );
    YItemSelector *		createMultiItemSelector	       ( YWidget * parent );
    virtual YItemSelector *	createCustomStatusItemSelector ( YWidget * parent, const YItemCustomStatusVector & customStates );

    virtual YMenuBar *          createMenuBar                  ( YWidget * parent );


protected:

    /**
     * Constructor.
     *
     * Use YUI::widgetFactory() to get the singleton for this class.
     **/
    YWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~YWidgetFactory();

}; // class YWidgetFactory


#endif // YWidgetFactory_h
