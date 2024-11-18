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

#include "YUI.h"
#include "YWidgetFactory.h"
#include "YDialog.h"
#include "YLayoutBox.h"
#include "YAlignment.h"
#include "YButtonBox.h"
#include "YPushButton.h"
#include "YInputField.h"
#include "YSpacing.h"
#include "YEvent.h"

#define YUILogComponent "ui-popup"
#include "YUILog.h"

#include "YPopupInternal.h"

using std::string;


void YPopupInternal::message(const string & label)
{
    auto f = YUI::widgetFactory();

    auto popup = f->createPopupDialog();
    auto mb = f->createMarginBox(popup, 1, 0.1);
    auto vbox = f->createVBox(mb);
    f->createLabel(vbox, label);

    auto bbox = f->createButtonBox(vbox);
    auto okButton = f->createPushButton(bbox, "OK");
    okButton->setRole(YOKButton);
    okButton->setDefaultButton();

    while (true)
    {
        auto event = popup->waitForEvent();
        if (event && (event->widget() == okButton || event->eventType() == YEvent::CancelEvent))
        {
            break;
        }
    }

    popup->destroy();
}

/**
 * Helper method for adding new input fields
 * @param parent Where to add the widget
 * @param val    The initial value
 */
static void addTextField(YWidget *parent, const string &val)
{
    auto new_item = YUI::widgetFactory()->createInputField(parent, "");
    new_item->setProperty("Value", YPropertyValue(val));
    new_item->setProperty("HStretch", YPropertyValue(true));
}

bool YPopupInternal::editStringArray(StringArray &array, const string &label)
{
    auto f = YUI::widgetFactory();

    auto popup = f->createPopupDialog();
    auto mb = f->createMarginBox(popup, 1, 0.1);
    auto vbox = f->createVBox(mb);
    f->createHeading(vbox, label);
    YWidget *arrayBox = f->createVBox(vbox);

    // access by reference
    for(auto&& str: array) addTextField(arrayBox, str);

    auto addButton = f->createPushButton(vbox, "Add Item");

    auto spacing = f->createVSpacing(vbox, 1);
    spacing->setProperty("VStretch", YPropertyValue(true));

    auto bbox = f->createButtonBox(vbox);
    auto okButton = f->createPushButton(bbox, "OK");
    okButton->setRole(YOKButton);
    okButton->setDefaultButton();
    auto cancelButton = f->createPushButton(bbox, "Cancel");
    cancelButton->setRole(YCancelButton);

    bool ret;

    while (true)
    {
        auto event = popup->waitForEvent();

        if (!event) continue;

        // cancel button or the window manager close button
        if (event->widget() == cancelButton || event->eventType() == YEvent::CancelEvent)
        {
            ret = false;
            break;
        }
        else if (event->widget() == okButton)
        {
            array.clear();

            // put all input field values into the target array
            for(auto&& widget: *arrayBox)
            {
                auto input = dynamic_cast<YInputField*>(widget);
                if (input) array.push_back(input->value());
            }

            ret = true;
            break;
        }
        else if (event->widget() == addButton)
        {
            addTextField(arrayBox, "");
            popup->recalcLayout();
        }
        else
            yuiWarning() << "Unknown event " << event << endl;
    }

    popup->destroy();

    return ret;
}

YPopupInternal::StringArray YPopupInternal::editNewStringArray(const string &label)
{
    YPopupInternal::StringArray ret { "", "", "" };

    if (editStringArray(ret, label))
        return ret;
    else
        // empty array
        return StringArray();
}
