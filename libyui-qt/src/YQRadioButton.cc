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

  File:	      YQRadioButton.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <qradiobutton.h>
#include <QMouseEvent>
#include <QBoxLayout>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include <yui/YEvent.h>
#include "YQRadioButton.h"
#include <yui/YRadioButtonGroup.h>
#include "YQSignalBlocker.h"

using std::string;
using std::endl;

#define SPACING 8

// +----+----------------------------------+----+
// |	|(o) RadioButtonlabel		   |	|
// +----+----------------------------------+----+
// <----> SPACING			   <---->



YQRadioButton::YQRadioButton( YWidget * 	parent,
			      const string & 	label,
			      bool 		checked )
    : QRadioButton( fromUTF8( label ), ( QWidget *) (parent->widgetRep() ) )
    , YRadioButton( parent, label )
{
    setWidgetRep( this );

    // QRadioButton uses its own logic by default to make sure that only one
    // button of a radio box is checked at any time, but this interferes with
    // YRadioButtonGroup. Let YRadioButtonGroup and YQRadioButton::changed()
    // handle this.
    setAutoExclusive( false );

    setChecked( checked );

    installEventFilter(this);

    connect ( this,     &pclass(this)::toggled,
	      this,	&pclass(this)::changed );
}


void
YQRadioButton::setUseBoldFont( bool useBold )
{
    setFont( useBold ?
             YQUI::yqApp()->boldFont() :
             YQUI::yqApp()->currentFont() );

    YRadioButton::setUseBoldFont( useBold );
}


int YQRadioButton::preferredWidth()
{
    return sizeHint().width();
}


int YQRadioButton::preferredHeight()
{
    return sizeHint().height();
}


void YQRadioButton::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQRadioButton::value()
{
    return isChecked();
}


void YQRadioButton::setValue( bool newValue )
{
    YQSignalBlocker sigBlocker( this );

    // yuiDebug() << "Setting " << this << (newValue ? " on" : " off") << endl;
    setChecked( newValue );

    if ( newValue )
    {
	YRadioButtonGroup * group = buttonGroup();

	if ( group )
	    group->uncheckOtherButtons( this );
    }
}


void YQRadioButton::setLabel( const string & label )
{
    setText( fromUTF8( label ) );
    YRadioButton::setLabel( label );
}


void YQRadioButton::setEnabled( bool enabled )
{
    QRadioButton::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQRadioButton::setKeyboardFocus()
{
    setFocus();

    return true;
}


void YQRadioButton::changed( bool newState )
{
    if ( newState )
    {
	yuiDebug() << "User set " << this << ( newState ? " on" : " off" ) << endl;
	YRadioButtonGroup * group = buttonGroup();

	if ( group )
	    group->uncheckOtherButtons( this );

	if ( notify() )
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
    }
    else
    {
        // prevent that the user deselects all items
        setChecked( true );
    }
}


bool YQRadioButton::eventFilter( QObject * obj, QEvent * event )
{
    if ( event && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (event);

        if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
        {
	    yuiMilestone() << "Right click on button detected" << endl;
	    YQUI::yqApp()->maybeLeftHandedUser();
        }
    }

    return QObject::eventFilter( obj, event );
}
