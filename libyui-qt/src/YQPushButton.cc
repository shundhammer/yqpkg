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

  File:	      YQPushButton.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "YQPushButton.h"

using std::string;


YQPushButton::YQPushButton( YWidget *		parent,
			    const string &	label )
    : YQGenericButton( parent, label )
{
    setWidgetRep( this );

    QPushButton * button = new QPushButton( fromUTF8( label ), this );
    Q_CHECK_PTR( button );

    setQPushButton( button );

    button->setMinimumSize( 2, 2 );
    button->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( button->minimumSize()
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );

    connect( button, &pclass(button)::clicked,
	     this,   &pclass(this)::hit );
}


YQPushButton::~YQPushButton()
{
    // NOP
}


int YQPushButton::preferredWidth()
{
    return 2 * YQButtonBorder + qPushButton()->sizeHint().width();
}


int YQPushButton::preferredHeight()
{
    return 2 * YQButtonBorder + qPushButton()->sizeHint().height();
}


void YQPushButton::setSize( int newWidth, int newHeight )
{
    qPushButton()->resize( newWidth  - 2 * YQButtonBorder,
			   newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


void YQPushButton::hit()
{
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


void YQPushButton::activate()
{
    hit();
}
