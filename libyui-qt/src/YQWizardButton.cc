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

  File:	      YQWizardButton.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQWizardButton.h"
#include "YQWizard.h"

using std::string;


YQWizardButton::YQWizardButton( YQWizard *	wizard,
				QWidget *	buttonParent,
				const string &	label )
    : YQGenericButton( wizard, label )
    , _wizard( wizard )
{
    QPushButton * button = new QPushButton( fromUTF8( label ), buttonParent );
    Q_CHECK_PTR( button );

    setQPushButton( button );
    setWidgetRep( button );

    connect( button, &pclass(button)::clicked,
	     this,   &pclass(this)::clicked );


    // This widget itself will never be visible, only its button - which is not
    // a child of this widget.
    QWidget::hide();
}


YQWizardButton::~YQWizardButton()
{
    // Don't delete qPushButton() here - its parent (buttonParent) will take
    // care of that!
}


void YQWizardButton::hide()
{
    if ( qPushButton() )
	qPushButton()->hide();
}


void YQWizardButton::show()
{
    if ( qPushButton() )
	qPushButton()->show();
}


bool YQWizardButton::isShown() const
{
    if ( qPushButton() )
	return !qPushButton()->isHidden();
    else
	return false;
}


bool YQWizardButton::isHidden() const
{
    return ! isShown();
}


int YQWizardButton::preferredWidth()
{
    // This widget doesn't have a YWidget-based visual representation, it's
    // only a YWidget for shortcut checking etc.

    return 0;
}


int YQWizardButton::preferredHeight()
{
    // This widget doesn't have a YWidget-based visual representation, it's
    // only a YWidget for shortcut checking etc.

    return 0;
}


void YQWizardButton::setSize( int newWidth, int newHeight )
{
    // NOP
}




