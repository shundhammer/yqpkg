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

  File:	      YQGenericButton.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#include <qevent.h>
#include <qevent.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include <yui/YEvent.h>
#include "YQGenericButton.h"
#include "YQDialog.h"

using std::string;
using std::endl;


YQGenericButton::YQGenericButton( YWidget *		parent,
				  const string &	label )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YPushButton( parent, label )
    , _dialog( 0 )
    , _qPushButton( 0 )
{
    setWidgetRep( 0 );
}


void YQGenericButton::setQPushButton( QPushButton * pb )
{
    _qPushButton = pb;
    _qPushButton->installEventFilter( this );
    _qPushButton->setAutoDefault( true );

    YPushButton::setLabel( toUTF8 ( _qPushButton->text() ) );
}


YQGenericButton::~YQGenericButton()
{
    if ( _dialog ) // If we don't have one any more, don't bother
    {
	if ( _dialog->focusButton() == this )
	    _dialog->losingFocus( this );

	if ( _dialog->defaultButton() == this )
	    _dialog->setDefaultButton(0);
    }
}


void YQGenericButton::forgetDialog()
{
    _dialog = 0;
}


YQDialog *
YQGenericButton::dialog()
{
    if ( ! _dialog )
    {
	YDialog * yDialog = findDialog();

	if ( yDialog )
	    _dialog = dynamic_cast<YQDialog *> (yDialog);

	YUI_CHECK_PTR( _dialog );
    }

    return _dialog;
}


void YQGenericButton::setEnabled( bool enabled )
{
    if ( _qPushButton )
	_qPushButton->setEnabled( enabled );

    YWidget::setEnabled( enabled );
}


bool YQGenericButton::isEnabled() const
{
    return _qPushButton ? _qPushButton->isEnabled() : false;
}


void YQGenericButton::setIcon( const string & iconName )
{
    if ( ! _qPushButton )
    {
	yuiError() << "NULL button (icon " << iconName << ")" << endl;
	return;
    }

    QString qIconName = fromUTF8( iconName );

    if ( qIconName.isEmpty() )
    {
	_qPushButton->setIcon( QIcon() );
	return;
    }

    // Search for the icon - FaTE #306356
    // qIconName =  fromUTF8( YQUI::yqApp()->iconLoader()->findIcon( iconName ) );
    // QPixmap icon( qIconName );
    // Use method from Qt instead
    QIcon icon = QIcon::fromTheme ( iconName.c_str() );

    if ( icon.isNull() )
	yuiWarning() << "Can't load icon \"" << qIconName << "\"" << endl;
    else
	_qPushButton->setIcon( icon );
}


void YQGenericButton::setLabel( const QString & label )
{
    if ( _qPushButton )
	_qPushButton->setText( label );
    else
	yuiError() << "NULL button \"" << label << "\"" << endl;

    YPushButton::setLabel( toUTF8( label ) );
}


void YQGenericButton::setLabel( const string & label )
{
    if ( _qPushButton )
	_qPushButton->setText( fromUTF8( label ) );
    else
	yuiError() << "NULL button \"" << label << "\"" << endl;

    YPushButton::setLabel( label );
}


void YQGenericButton::showAsDefault( bool show )
{
    if ( _qPushButton )
    {
        _qPushButton->setAutoDefault( !show );
	_qPushButton->setDefault( show );
	_qPushButton->update();
    }
}


bool YQGenericButton::isShownAsDefault() const
{
    return _qPushButton ? _qPushButton->isDefault() : false;
}


QString
YQGenericButton::text() const
{
    return _qPushButton ? _qPushButton->text() : "";
}


void YQGenericButton::activate()
{
    if ( _qPushButton )
	_qPushButton->animateClick();
}


bool YQGenericButton::eventFilter( QObject * obj, QEvent * event )
{
    if ( event )
    {
	if ( event->type() == QEvent::FocusIn )
	{
	    dialog()->gettingFocus( this );
	    return false;	// event processed?
	}
	else if ( event->type() == QEvent::FocusOut )
	{
	    dialog()->losingFocus( this );
	    return false;	// event processed?
	}
	else if ( event->type() == QEvent::MouseButtonRelease )
	{
	    QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (event);

	    if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
	    {
		yuiMilestone() << "Right click on button detected" << endl;
		YQUI::yqApp()->maybeLeftHandedUser();
	    }
	}
    }


    return QObject::eventFilter( obj, event );
}


bool YQGenericButton::setKeyboardFocus()
{
    if ( ! _qPushButton )
	return false;

    dialog()->gettingFocus( this );
    _qPushButton->setFocus();

    return true;
}

void YQGenericButton::setShortcut ( const QKeySequence & key )
{
    _qPushButton->setShortcut (key );
}


