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

  File:		YDialogSpy.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <sstream>

#define YUILogComponent "ui-dialog-spy"
#include "YUILog.h"

#include "YDialogSpy.h"
#include "YWidgetFactory.h"
#include "YWidgetID.h"
#include "YDialog.h"
#include "YEvent.h"
#include "YTable.h"
#include "YTree.h"
#include "YTreeItem.h"
#include "YLayoutBox.h"
#include "YAlignment.h"
#include "YButtonBox.h"
#include "YPushButton.h"
#include "YMenuButton.h"
#include "YComboBox.h"
#include "YInputField.h"
#include "YCheckBox.h"
#include "YRadioButton.h"
#include "YProgressBar.h"
#include "YRichText.h"
#include "YBusyIndicator.h"
#include "YSelectionBox.h"
#include "YMultiSelectionBox.h"
#include "YMultiLineEdit.h"
#include "YLabel.h"
#include "YLogView.h"
#include "YIntField.h"
#include "YImage.h"
#include "YSpacing.h"
#include "YFrame.h"
#include "YEmpty.h"
#include "YPackageSelector.h"
#include "YReplacePoint.h"
#include "YPropertyEditor.h"
#include "YPopupInternal.h"
#include "YAlignment.h"
#include "YCheckBoxFrame.h"
#include "YRadioButtonGroup.h"
#include "YUI.h"

#define TREE_VWEIGHT	40
#define PROP_VWEIGHT	60

#define DIA_HEIGHT	24

#define TREE_HEIGHT	10
#define TREE_WIDTH	50

#define PROP_HEIGHT	12
#define PROP_WIDTH	50

using std::string;


/**
 * Custom tree item class to map tree items to widgets
 **/
class YWidgetTreeItem: public YTreeItem
{
public:

    YWidgetTreeItem( YWidget *	widget,
		     bool	isOpen )
	: YTreeItem( "", isOpen )
	, _widget( widget )
    {
	setWidgetLabel();
    }

    YWidgetTreeItem( YWidgetTreeItem *	parent,
		     YWidget *		widget,
		     bool		isOpen )
	: YTreeItem( parent, "", isOpen )
	, _widget( widget )
    {
	setWidgetLabel();
    }

    virtual ~YWidgetTreeItem() {}

    YWidget * widget() const { return _widget; }


protected:

    void setWidgetLabel()
    {
        std::ostringstream str;
	str << _widget;
	setLabel( str.str() );
    }

private:

    YWidget * _widget;
};


static void fillTree( YWidgetTreeItem *		parent,
		      YWidgetListConstIterator	begin,
		      YWidgetListConstIterator	end,
		      int			treeLevel );



class YDialogSpyPrivate
{
public:

    YDialogSpyPrivate()
	: targetDialog( nullptr )
	, spyDialog( nullptr )
	, widgetTree( nullptr )
	, propButton( nullptr )
	, propReplacePoint( nullptr )
	, propTable( nullptr )
	{}

    ~YDialogSpyPrivate();

    YDialog *		targetDialog;	// Dialog that is being inspected
    YDialog *		spyDialog;	// Debug dialog that shows widget data
    YTree *		widgetTree;	// Tree widget to show widget hierarchy
    YPushButton *	propButton;
    YMenuButton *	addButton;
    YPushButton *	deleteButton;
    YPushButton *	upButton;
    YPushButton *	downButton;
    YReplacePoint *	propReplacePoint;
    YTable *		propTable;
    // YMenuItem *		exportMenu;
    YMenuItem *		closeItem;


    YWidget * selectedWidget();
    void selectedWidgetChanged();
    void refreshProperties();
    bool toggleProperties();
    void highlightWidget( bool enable = true );

    void deleteWidget();
    void addWidget( const string & type );
    void editProperty();
    void moveSelectedUp()   { moveSelected(MOVE_UP);   }
    void moveSelectedDown() { moveSelected(MOVE_DOWN); }

private:

    enum Direction
    {
	MOVE_UP = 0,
	MOVE_DOWN
    };

    void moveSelected( Direction direction );
    void showProperties();
    void hideProperties();
    bool propertiesShown() const;
    void targetDialogUpdated();
    void refreshButtonStates();
    void editWidget( YWidget *widget, const string & property="Label" );
};


/**
 * Destructor - switch off widget highlighting at the end
 */
YDialogSpyPrivate::~YDialogSpyPrivate()
{
    highlightWidget( false );
}


/**
 * Fill the widget tree content
 *
 * @param target the target dialog which will be examined
 * @param widgetTree where to display the structure
*/
void fillWidgetTree( YDialog *target, YTree *widgetTree )
{
    YWidgetTreeItem * rootItem = new YWidgetTreeItem( target, true );
    YUI_CHECK_NEW( rootItem );

    fillTree( rootItem, target->childrenBegin(), target->childrenEnd(), 1 );
    widgetTree->addItem( rootItem );
    widgetTree->rebuildTree();
}


/**
 * Constructor - create the main spy dialog
 */
YDialogSpy::YDialogSpy( YDialog * targetDialog )
    : priv( new YDialogSpyPrivate() )
{
    if ( ! targetDialog )
	targetDialog = YDialog::topmostDialog();

    priv->targetDialog = targetDialog;
    YWidgetFactory * fac = YUI::widgetFactory();

    priv->spyDialog	 = fac->createPopupDialog();
    YAlignment * diaMin	 = fac->createMinHeight( priv->spyDialog, DIA_HEIGHT );
    YLayoutBox * vbox	 = fac->createVBox( diaMin );

    auto alignment = fac->createLeft( vbox );
    auto fileMenu  = fac->createMenuButton( alignment, "&File" );

    YItemCollection items;
    // priv->exportMenu = new YMenuItem( "Export (TODO)" );
    // items.push_back( priv->exportMenu );
    priv->closeItem =  new YMenuItem( "Close" );
    items.push_back( priv->closeItem );
    fileMenu->addItems( items );

    auto minSize = fac->createMinSize( vbox, TREE_WIDTH, TREE_HEIGHT );
    minSize->setWeight( YD_VERT, TREE_VWEIGHT );
    priv->widgetTree = fac->createTree( minSize, "Widget &Tree", false );
    priv->widgetTree->setNotify( true );

    fillWidgetTree(priv->targetDialog, priv->widgetTree);

    auto hbox = fac->createHBox( vbox );
    priv->propButton = fac->createPushButton( hbox, "&Properties >>>" );
    priv->addButton = fac->createMenuButton( hbox, "&Add" );

    YItemCollection add_items;
    YMenuItem *menu_info        = new YMenuItem( "Info" );
    YMenuItem *menu_buttons     = new YMenuItem( "Buttons" );
    YMenuItem *menu_input       = new YMenuItem( "Input" );
    YMenuItem *menu_align       = new YMenuItem( "Alignment" );
    YMenuItem *menu_size        = new YMenuItem( "Size" );
    YMenuItem *menu_containers  = new YMenuItem( "Containers" );
    YMenuItem *menu_special     = new YMenuItem( "Special" );

    add_items.push_back( menu_info );
    add_items.push_back( menu_buttons );
    add_items.push_back( menu_input );
    add_items.push_back( menu_align );
    add_items.push_back( menu_size );
    add_items.push_back( menu_containers );
    add_items.push_back( menu_special );

    new YMenuItem( menu_info, "Label" );
    new YMenuItem( menu_info, "Heading" );
    new YMenuItem( menu_info, "RichText" );
    new YMenuItem( menu_info, "ProgressBar" );
    new YMenuItem( menu_info, "BusyIndicator" );
    new YMenuItem( menu_info, "Table" );

    new YMenuItem( menu_buttons, "PushButton" );
    new YMenuItem( menu_buttons, "CheckBox" );
    new YMenuItem( menu_buttons, "ComboBox" );
    new YMenuItem( menu_buttons, "MenuButton" );
    new YMenuItem( menu_buttons, "RadioButton" );

    new YMenuItem( menu_input, "InputField" );
    new YMenuItem( menu_input, "IntField" );
    new YMenuItem( menu_input, "MultiLineEdit" );
    new YMenuItem( menu_input, "MultiSelectionBox" );
    new YMenuItem( menu_input, "Password" );
    new YMenuItem( menu_input, "SelectionBox" );

    new YMenuItem( menu_align, "Left" );
    new YMenuItem( menu_align, "Right" );
    new YMenuItem( menu_align, "Top" );
    new YMenuItem( menu_align, "Bottom" );
    new YMenuItem( menu_align, "HCenter" );
    new YMenuItem( menu_align, "VCenter" );
    new YMenuItem( menu_align, "HVCenter" );

    new YMenuItem( menu_size, "MinHeight" );
    new YMenuItem( menu_size, "MinWidth" );
    new YMenuItem( menu_size, "MinSize" );
    new YMenuItem( menu_size, "HSquash" );
    new YMenuItem( menu_size, "VSquash" );
    new YMenuItem( menu_size, "HVSquash" );
    new YMenuItem( menu_size, "HWeight" );
    new YMenuItem( menu_size, "VWeight" );

    new YMenuItem( menu_containers, "MarginBox" );
    new YMenuItem( menu_containers, "ButtonBox" );
    new YMenuItem( menu_containers, "CheckBoxFrame" );
    new YMenuItem( menu_containers, "Frame" );
    new YMenuItem( menu_containers, "HBox" );
    new YMenuItem( menu_containers, "HSpacing" );
    new YMenuItem( menu_containers, "ReplacePoint" );
    new YMenuItem( menu_containers, "VBox" );
    new YMenuItem( menu_containers, "VSpacing" );

    // TODO: these are not available in ncurses UI
    new YMenuItem( menu_special, "BarGraph" );
    new YMenuItem( menu_special, "DateField" );
    new YMenuItem( menu_special, "DumbTab" );
    new YMenuItem( menu_special, "Graph" );
    new YMenuItem( menu_special, "Slider" );
    new YMenuItem( menu_input,   "TimeField" );
    new YMenuItem( menu_special, "TimezoneSelector" );

    priv->addButton->addItems( add_items );

    priv->deleteButton = fac->createPushButton( hbox, "&Delete" );
    priv->upButton     = fac->createPushButton( hbox, "⬆ Up" );
    priv->downButton   = fac->createPushButton( hbox, "⬇ Down" );

    priv->propReplacePoint = fac->createReplacePoint( vbox );
    fac->createEmpty( priv->propReplacePoint );

    priv->selectedWidgetChanged();
}


/**
 * Destructor
 */
YDialogSpy::~YDialogSpy()
{
    if ( priv->spyDialog )
	priv->spyDialog->destroy();
}


/**
 * Is the property dialog displayed?
 *
 * @return true if the dialog is displayed
 */
bool YDialogSpyPrivate::propertiesShown() const
{
    return propTable != nullptr;
}


/**
 * Highlight the currently selected widget in the spy dialog
 */
void YDialogSpyPrivate::highlightWidget(bool enable)
{
    if (targetDialog) targetDialog->highlight( enable ? selectedWidget() : nullptr );
}


/**
 * Display details about the currently selected widget
 */
void YDialogSpyPrivate::showProperties()
{
    if ( propertiesShown() ) return;

	propReplacePoint->deleteChildren();
	propReplacePoint->setWeight( YD_VERT, PROP_VWEIGHT );

	auto fac = YUI::widgetFactory();
	auto minSize = fac->createMinSize( propReplacePoint, PROP_WIDTH, PROP_HEIGHT );
	auto header = new YTableHeader();
	YUI_CHECK_NEW( header );
	header->addColumn( "Property" );
	header->addColumn( "Value" );
	header->addColumn( "Type" );

	propTable = fac->createTable( minSize, header );
	propTable->setNotify( true );

	propButton->setLabel( "<<< &Properties" );
	propReplacePoint->showChild();
	spyDialog->recalcLayout();
}


/**
 * Hide property details
 */
void YDialogSpyPrivate::hideProperties()
{
    if ( !propertiesShown() ) return;

	propReplacePoint->deleteChildren();
	propReplacePoint->setWeight( YD_VERT, 0 );
	propTable = nullptr;
	YUI::widgetFactory()->createEmpty( propReplacePoint );

	propButton->setLabel( "&Properties >>>" );
	propReplacePoint->showChild();
	spyDialog->recalcLayout();
}

/**
 * Hide or show the properties dialog
 * @return true if the dialog is now displayed
 */
bool YDialogSpyPrivate::toggleProperties()
{
    bool ret = !propertiesShown();

    if (ret)
    {
	showProperties();
	refreshProperties();
    }
    else
	hideProperties();

    return ret;
}


/**
 * Refresh the displayed properties
 */
void YDialogSpyPrivate::refreshProperties()
{
    // properties shown?
    if ( !propTable )
	return;

    propTable->deleteAllItems();
    auto widget = selectedWidget();

    if ( !widget )
	return;

    YItemCollection items;
    auto propSet = widget->propertySet();
    items.reserve( propSet.size() );

    for ( YPropertySet::const_iterator it = propSet.propertiesBegin();
	  it != propSet.propertiesEnd();
	  ++it )
    {
	YProperty	prop	= *it;
	YPropertyValue	propVal = widget->getProperty( prop.name() );
	string	propValStr;

	switch ( prop.type() )
	{
	    case YStringProperty:
		propValStr = propVal.stringVal();
		break;

	    case YBoolProperty:
		propValStr = propVal.boolVal() ? "true" : "false";
		break;

	    case YIntegerProperty:
		propValStr = std::to_string(propVal.integerVal());
		break;

	    default:
		propValStr = "???";
		break;
	}

	auto item = new YTableItem( prop.name(), propValStr, prop.typeAsStr() );
	YUI_CHECK_NEW( item );
	items.push_back( item );
    }

    propTable->addItems( items );
    propTable->deselectAllItems();
}

/**
 * Fill the widget tree dialog
 *
 * @param parent    widget tree item
 * @param begin	    iterator pointing to the first item
 * @param end	    iterator pointing to the last item
 * @param treeLevel current tree level (nesting)
 */
void fillTree( YWidgetTreeItem *		parent,
	       YWidgetListConstIterator		begin,
	       YWidgetListConstIterator		end,
	       int				treeLevel )
{
    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YWidget * widget = *it;
	auto item = new YWidgetTreeItem( parent, widget, treeLevel < 4 );

	if ( widget->hasChildren() )
	    fillTree( item, widget->childrenBegin(), widget->childrenEnd(), treeLevel+1 );
    }
}


/**
 * The main loop of the spy dialog
 */
void YDialogSpy::exec()
{
    YUI_CHECK_PTR( priv->spyDialog );

    while ( true )
    {
	auto event = priv->spyDialog->waitForEvent();
	// yuiDebug() << "event: " << event << endl;
	if ( ! event ) continue;

	// window manager "close window" button
	if ( event->eventType() == YEvent::CancelEvent )
            break;
	else if ( event->eventType() == YEvent::MenuEvent)
	{
	    YMenuItem * menu_item = dynamic_cast<YMenuItem *>( event->item() );

	    // TODO: handle the export menu item
	    // if (menu_item == priv->exportMenu) continue;

	    // closing from menu
	    if ( menu_item == priv->closeItem )
                break;

	    // handle all unhandled menu items as "Add" menu items, this is much
	    // simpler than comparing it with the huge amount of menu item pointers
	    if ( menu_item )
	    {
		auto menu_label = menu_item->label();
		// yuiDebug() << "Activated menu item: " << menu_label << endl;
		priv->addWidget( menu_label );
	    }

	    continue;
	}

	// just make sure we do not use NULL in some unexpected case
	if ( !event->widget() )
            continue;

	if      ( event->widget() == priv->upButton     ) priv->moveSelectedUp();
	else if ( event->widget() == priv->downButton   ) priv->moveSelectedDown();
	else if ( event->widget() == priv->propButton   ) priv->toggleProperties();
	else if ( event->widget() == priv->deleteButton ) priv->deleteWidget();
	else if ( event->widget() == priv->propTable    ) priv->editProperty();
	else if ( event->widget() == priv->widgetTree   ) priv->selectedWidgetChanged();
    }
}


/**
 * Run the spy dialog for selected UI dialog
 * @param dialog UI dialog to examine
 */
void YDialogSpy::showDialogSpy( YDialog * dialog )
{
    try
    {
	YDialogSpy dialogSpy( dialog );
	dialogSpy.exec();
    }
    catch ( YUIException & exception )
    {
	// ignore all YUI exceptions which might happen when playing with the layout
	YUI_CAUGHT( exception );
	YPopupInternal::message("Error:\n" + exception.msg());
    }
}


/**
 * The currently selected wiget
 * @return The currently selected widget (or nullptr if nothing is selected)
 */
YWidget * YDialogSpyPrivate::selectedWidget()
{
    auto item = dynamic_cast<YWidgetTreeItem *>( widgetTree->selectedItem() );

    return item ? item->widget() : nullptr;
}


/**
 * The selected item has been changed, refresh the UI
 */
void YDialogSpyPrivate::selectedWidgetChanged()
{
    highlightWidget();
    refreshProperties();
    refreshButtonStates();
}


/**
 * Run the property editor for the current widget
 */
void YDialogSpyPrivate::editProperty()
{
    auto selected_item = dynamic_cast<YTableItem *>( propTable->selectedItem() );

    if ( ! selected_item )
        return;

    auto cell = selected_item->cell(0);
    yuiMilestone() << "editing property: " << cell->label();

    YPropertyEditor editor(selectedWidget());

    // update the property table when only the property has been changed
    if ( editor.edit( cell->label() ) )
        refreshProperties();
}


/**
 * Delete the currently selected widget
 */
void YDialogSpyPrivate::deleteWidget()
{
    auto w = selectedWidget();

    if ( ! w )
        return;

    auto parent = w->parent();

    if ( ! parent )
        return;

    yuiMilestone() << "removing widget: " << w << endl;
    parent->removeChild(w);

    if ( w->isValid() )
    {
	delete w;
    }

    // any other child left after the removal?
    if ( ! parent->hasChildren() )
    {
	// add an Empty widget to have a valid widget tree
	// e.g. empty VBoxes are not allowed
	YUI::widgetFactory()->createEmpty( parent );
    }

    targetDialogUpdated();
}

/**
 * Helper method - Is the widget a VBox or Hbox?
 *
 * @param  widget the widget
 * @return true if the widget is a VBox or HBox
 */
bool isBox(const YWidget *widget)
{
    return dynamic_cast<const YLayoutBox *>( widget );
}


/**
 * Helper method - Is the widget a VBox?
 *
 * @param  widget the widget
 * @return true if the widget is a VBox
 */
bool isVBox( const YWidget *widget )
{
    auto box = dynamic_cast<const YLayoutBox *>( widget );

    return box && box->primary() == YD_VERT;
}


/**
 * Move the selected widget up/left or down/right. The visual direction
 * actually depends on the widget, it just moves the widget to the begining
 * or the end of the container.
 *
 * @param true = up move to the begining (up/left), false = to the end (down/right)
 */
void YDialogSpyPrivate::moveSelected( Direction direction )
{
    auto target_widget = selectedWidget();

    if ( ! target_widget )
        return;

    auto parent = target_widget->parent();

    if ( ! parent || ! isBox( parent ) )
        return;

    if ( direction == MOVE_UP )
    {
	// the first child cannot be moved further
	if ( target_widget == parent->firstChild() )
            return;

	auto i = find( parent->childrenBegin(), parent->childrenEnd(), target_widget );

	if ( i != parent->childrenEnd() )
	{
	    // swap with the preceeding widget
	    // Note: use a temporary variable to not rely on the argument evaluation order!
	    auto other = i--;
            std::swap( *other, *i );
	}
    }
    else // moving down
    {
	// the last child cannot be moved further to the end
	if ( target_widget == parent->lastChild() )
            return;

	auto i = find( parent->childrenBegin(), parent->childrenEnd(), target_widget );

	if ( i != parent->childrenEnd() )
	{
	    // swap with the succeeding widget
	    // Note: use a temporary variable to not rely on the argument evaluation order!
	    auto other = i++;
            std::swap(*other, *i);
	}
    }

    targetDialogUpdated();
}

/**
 * Generic handler for adding widgets
 *
 * @param type Type of the widget to add
 */
void YDialogSpyPrivate::addWidget( const string & type )
{
    auto widget = selectedWidget();

    if ( ! widget )
        return;

    try
    {
	auto f = YUI::widgetFactory();

	if (type == "Bottom")
	    editWidget( f->createBottom( widget ) );
	else if (type == "BusyIndicator")
	    editWidget( f->createBusyIndicator( widget, "Busy Indicator", 10000 ) );
	else if (type == "ButtonBox")
	    editWidget( f->createButtonBox( widget ) );
	else if (type == "ComboBox")
	{
	    auto cb = f->createComboBox( widget, "Combo Box" );
	    editWidget(cb);

	    YPopupInternal::StringArray items( YPopupInternal::editNewStringArray( "Menu Items" ) );

	    YItemCollection add_items;
	    // access by reference
	    for ( auto && str: items )
                add_items.push_back( new YMenuItem( str ) );

	    cb->addItems( add_items );
	}
	else if ( type == "Empty" )
	    editWidget( f->createEmpty( widget ) );
	else if ( type == "Frame" )
	    editWidget( f->createFrame( widget, "Frame" ) );
	else if ( type == "HBox" )
	    editWidget( f->createHBox( widget ) );
	else if ( type == "Heading" )
	    editWidget( f->createHeading( widget, "Heading" ) );
	else if ( type == "HSpacing" )
	    editWidget( f->createHSpacing( widget ) );
	else if ( type == "HStretch" )
	    editWidget( f->createHStretch( widget ) );
	else if ( type == "CheckBox" )
	    editWidget( f->createCheckBox( widget, "Check Box" ) );
	else if ( type == "CheckBoxFrame" )
	    // make it checked by default
	    editWidget( f->createCheckBoxFrame( widget, "Check Box Frame", true ) );
	else if ( type == "Image" )
	    editWidget( f->createImage( widget, "" ) );
	else if ( type == "InputField" )
	    editWidget( f->createInputField( widget, "Input" ) );
	else if ( type == "IntField" )
	    editWidget( f->createIntField( widget, "Integer Field", 0, 100, 50 ) );
	else if ( type == "Label" )
	    editWidget( f->createLabel( widget, "Label" ) );
	else if ( type == "Left" )
	    editWidget( f->createLeft( widget ) );
	else if ( type == "LogView" )
	    editWidget( f->createLogView( widget, "Log View", 12 ) );
	else if ( type == "MenuButton" )
	{
	    auto menu = f->createMenuButton( widget, "Menu" );
	    editWidget(menu);

	    YPopupInternal::StringArray items( YPopupInternal::editNewStringArray("Menu Items") );

	    YItemCollection add_items;
	    // access by reference
	    for ( auto && str: items )
                add_items.push_back( new YMenuItem( str ) );

	    menu->addItems( add_items );
	}
	else if ( type == "MinHeight" )
	    editWidget( f->createMinHeight( widget, 10) );
	else if ( type == "MinWidth" )
	    editWidget( f->createMinWidth( widget, 10) );
	else if ( type == "MinSize" )
	    editWidget( f->createMinSize( widget, 10, 10) );
	else if ( type == "MultiLineEdit" )
	    editWidget( f->createMultiLineEdit( widget, "MultiLineEdit" ) );
	else if ( type == "MultiSelectionBox" )
	{
	    auto msb = f->createMultiSelectionBox( widget, "MultiSelection Box" );
	    editWidget( msb);

	    // edit the item list and update the widget after pressing OK
	    YPopupInternal::StringArray items( YPopupInternal::editNewStringArray( "Items" ) );
	    // access by reference
	    for ( auto && str: items )
                msb->addItem(str);
	}
	else if ( type == "OutputField" )
	    editWidget( f->createOutputField( widget, "Output Field" ) );
	else if ( type == "Password" )
	    editWidget( f->createPasswordField( widget, "Password" ) );
	else if ( type == "ProgressBar" )
	    editWidget( f->createProgressBar( widget, "Progress" ) );
	else if ( type == "PushButton" )
	    editWidget( f->createPushButton( widget, "Button" ) );
	else if ( type == "RadioButton" )
	    editWidget( f->createRadioButton( widget, "Radio Button" ) );
	else if ( type == "RadioButtonGroup" )
	    editWidget( f->createRadioButtonGroup( widget ) );
	else if ( type == "ReplacePoint" )
	    editWidget( f->createReplacePoint( widget ) );
	else if ( type == "Right" )
	    editWidget( f->createRight( widget ) );
	else if ( type == "RichText" )
	    editWidget( f->createRichText( widget, "This is a <b>RichText</b>." ) );
	else if ( type == "SelectionBox" )
	    editWidget( f->createSelectionBox( widget, "Selection Box" ) );
	else if ( type == "Table" )
	{
	    YPopupInternal::StringArray items( YPopupInternal::editNewStringArray( "Table Columns" ) );

	    // abort adding if Cancel has been pressed
	    if ( ! items.empty() )
	    {
		auto header = new YTableHeader();

		// access by reference
		for ( auto && str: items )
                    header->addColumn(str);

		editWidget( f->createTable( widget, header ) );
	    }
	}
	else if ( type == "Top" )
	    editWidget( f->createTop( widget ) );
	else if ( type == "Tree" )
	    editWidget( f->createTree( widget, "Tree" ) );
	else if ( type == "VBox" )
	    editWidget( f->createVBox( widget ) );
	else if ( type == "VSpacing" )
	    editWidget( f->createVSpacing( widget ) );
	else if ( type == "VStretch" )
	    editWidget( f->createVStretch( widget ) );
	else
	{
	    YPopupInternal::message( "Adding \"" + type + "\" widget type is not supported." );
	    return;
	}

	targetDialogUpdated();
    }
    catch ( const YUIException & exception )
    {
	YPopupInternal::message( "Could not add a new widget:\n" + exception.msg() );
    }
}

/**
 * Refresh the target dialog after modifying it.
 */
void YDialogSpyPrivate::targetDialogUpdated()
{
    // redraw the target dialog
    targetDialog->recalcLayout();

    // refresh the spy dialog
    widgetTree->deleteAllItems();
    fillWidgetTree( targetDialog, widgetTree );
}

/**
 * Refresh button states in the main spy dialog
 */
void YDialogSpyPrivate::refreshButtonStates()
{
    auto widget = selectedWidget();
    auto parent = widget ? widget->parent() : nullptr;

    // Enable the moving buttons ony when the selected widget is inside
    // a VBox/HBox container, set the labels according to stacking direction.
    if (widget && parent && isBox(parent))
    {
	upButton->setEnabled( widget != parent->firstChild() );
	upButton->setLabel( isVBox(parent) ? "⬆ Up" : "⬅ Left" );

	downButton->setEnabled( widget != parent->lastChild() );
	downButton->setLabel( isVBox(parent) ? "⬇ Down" : "➡ Right" );
    }
    else
    {
	upButton->setEnabled( false );
	downButton->setEnabled( false );
    }

    // TODO: Enable the [Add] menu button only when a widget can be added
    // inside the current widget (i.e. it is a container). Check the widget's
    // child manager wheter it is YSingleWidgetChildManager or a YWidgetChildrenRejector.

    // Disable the [Delete] button when for the top level widget (YDialog)
    // TODO: disable it for the YQWizardButtons (Next, Back, ...), they cannot be
    // removed from the dialog.
    deleteButton->setEnabled(parent);
}

/**
 * Edit widget property
 * @param widget   selected widget
 * @param property property name
 */
void YDialogSpyPrivate::editWidget( YWidget *widget, const string &property )
{
    // redraw the target dialog
    targetDialog->recalcLayout();

    if ( ! widget->propertySet().contains(property) )
        return;

    YPropertyEditor editor( widget );
    editor.edit( property );
}
