/*
  Copyright (C) 2016 SUSE LLC

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

#include "YPropertyEditor.h"

#include "YUI.h"
#include "YWidgetFactory.h"
#include "YDialog.h"
#include "YLayoutBox.h"
#include "YAlignment.h"
#include "YButtonBox.h"
#include "YPushButton.h"
#include "YEvent.h"
#include "YPopupInternal.h"
#include "YComboBox.h"
#include "YInputField.h"
#include "YIntField.h"

#include <limits>

#define YUILogComponent "ui-property-editor"
#include "YUILog.h"

using std::string;


class YPropertyEditorPriv
{
public:

    YPropertyEditorPriv(YWidget * widget) : _widget(widget),
                                            popup(nullptr), combo(nullptr), intfield(nullptr),
                                            input(nullptr), okButton(nullptr), cancelButton(nullptr)
        {}

    bool edit( const string &property );

private:

    /**
     * show the dialog on the screen
     * @param property Name of the property to edit
     */
    void show( const string &property );

    /**
     * Run the main event loop
     * @param  property Name of the property to edit
     * @return true if the value has been changed, false otherwise
     */
    bool run( const string &property );

    /**
     * Close the dialog window
     */
    void close();

    /**
     * Refresh the dialog conatining the widget
     */
    void refreshDialog();

    YWidget * _widget;

    /**
     * Is the property read-only?
     * @param  property Name of the property
     * @return true if it is read-only, false if it can be changed
     */
    bool isReadOnly( const string &property );

    // UI widgets
    // the main popup
    YDialog *popup;

    // input widgets
    YComboBox *combo;
    YIntField *intfield;
    YInputField *input;

    // buttons
    YPushButton *okButton;
    YPushButton *cancelButton;

    /**
     * Is the property editable? Editable property is not read-only and
     * it is String, Integer or Boolean type.
     * @param  property Name of the property
     * @return true if the property can be changed
     */
    bool editable(const string &property);
};


/**
 * Helper method - refresh the dialog containing the widget
 * @param widget [description]
 */
void YPropertyEditorPriv::refreshDialog()
{
    auto dialog = _widget->findDialog();
    if (dialog) dialog->recalcLayout();
}


bool YPropertyEditorPriv::edit(const string &property)
{
    if (!_widget || !editable(property)) return false;

    yuiMilestone() << "editing property \"" << property << "\" (type: " <<
        _widget->getProperty(property).typeAsStr() << ")";

    show(property);
    bool changed = run(property);
    close();

    return changed;
}


bool YPropertyEditorPriv::isReadOnly(const string &property)
{
    // is the property read-only?
    YPropertySet propSet = _widget->propertySet();
    for ( YPropertySet::const_iterator it = propSet.propertiesBegin();
          it != propSet.propertiesEnd();
          ++it )
    {
        YProperty prop = *it;

        if (prop.name() == property)
        {
            return prop.isReadOnly();
        }
    }

    // we cannot edit an unknown property, throw an exception
    YUI_THROW( YUIException( "Unknown property: " + property) );

    // FIXME: never reached, just make the compiler happy (can it be improved?)
    return false;
}


void YPropertyEditorPriv::show(const string &property)
{
    YPropertyValue prop_value = _widget->getProperty(property);
    YPropertyType type = prop_value.type();

    auto f = YUI::widgetFactory();

    popup = f->createPopupDialog();
    auto vbox = f->createVBox(popup);

    if (type == YBoolProperty)
    {
        combo = f->createComboBox(vbox, property);
        combo->setNotify(true);

        YItemCollection items;
        items.push_back(new YItem("true"));
        items.push_back(new YItem("false"));
        combo->addItems(items);
        combo->setValue(prop_value.boolVal() ? "true" : "false");
    }
    else if (type == YIntegerProperty)
    {
        intfield = f->createIntField(vbox, property,
                                     // we do not know anything about that property so use the
                                     // max int and min int values here
                                     std::numeric_limits<int>::min(),
                                     std::numeric_limits<int>::max(),
                                     prop_value.integerVal());
        intfield->setNotify(true);
    }
    else if (type == YStringProperty)
    {
        input = f->createInputField(vbox, property);
        input->setNotify(true);
        input->setValue(prop_value.stringVal());
    }

    auto bbox = f->createButtonBox(vbox);
    okButton = f->createPushButton(bbox, "OK");
    okButton->setRole(YOKButton);
    okButton->setDefaultButton();
    cancelButton = f->createPushButton(bbox, "Cancel");
    cancelButton->setRole(YCancelButton);
}


void YPropertyEditorPriv::close()
{
    popup->destroy();

    // nullify the widget pointers, just to be sure...
    popup = NULL;
    okButton = NULL;
    cancelButton = NULL;
    combo = NULL;
    intfield = NULL;
    input = NULL;
}


bool YPropertyEditorPriv::run(const string &property)
{
    // backup the original property value so it can be restored after
    // clicking the [Cancel] button later
    YPropertyValue orig = _widget->getProperty(property);;

    while (true)
    {
        YEvent * event = popup->waitForEvent();
        if (event)
        {
            if (event->widget() == cancelButton || event->eventType() == YEvent::CancelEvent)
            {
                // restore the original value
                if (_widget->getProperty(property) != orig)
                {
                    _widget->setProperty(property, orig);
                    refreshDialog();
                }

                // not modified
                return false;
            }
            else if (event->widget() == okButton)
            {
                return _widget->getProperty(property) != orig;
            }
            else if (event->widget() == combo)
            {
                string value = combo->value();
                yuiMilestone() << "Value changed to " << value;
                _widget->setProperty(property, YPropertyValue(value == "true"));
                refreshDialog();
            }
            else if (event->widget() == input)
            {
                string value = input->value();
                yuiMilestone() << "Value changed to " << value;

                _widget->setProperty(property, YPropertyValue(value));
                refreshDialog();
            }
            else if (event->widget() == intfield)
            {
                int value = intfield->value();
                yuiMilestone() << "Value changed to " << value;

                _widget->setProperty(property, YPropertyValue(value));
                refreshDialog();
            }
        }
    }
}

bool YPropertyEditorPriv::editable(const string &property)
{
    YPropertyValue prop_value = _widget->getProperty(property);

    // is the property read-only?
    if (isReadOnly(property))
    {
        YPopupInternal::message("Property \"" + property + "\" is read only!");
        return false;
    }

    YPropertyType type = prop_value.type();
    // edit special properties cannot be edited
    if (type != YBoolProperty && type != YStringProperty && type != YIntegerProperty)
    {
        return false;
    }

    return true;
}


bool YPropertyEditor::edit(const string &property)
{
    return priv->edit(property);
}


YPropertyEditor::YPropertyEditor(YWidget * widget)
    : priv(new YPropertyEditorPriv(widget))
{
}


YPropertyEditor::~YPropertyEditor()
{
}
