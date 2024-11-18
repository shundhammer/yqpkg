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

  File:	      YQInputField.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  Textdomain "qt"

/-*/


#include <qlineedit.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "QY2CharValidator.h"
#include "YQInputField.h"
#include "YQi18n.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>

using std::string;
using std::endl;



YQInputField::YQInputField( YWidget * 		parent,
			    const string &	label,
			    bool		passwordMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YInputField( parent, label, passwordMode )
    , _validator(0)
    , _displayingCapsLockWarning( false )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_lineEdit = new YQRawLineEdit( this );
    YUI_CHECK_NEW( _qt_lineEdit );
    layout->addWidget( _qt_lineEdit );

    _caption->setBuddy( _qt_lineEdit );

    connect( _qt_lineEdit, &pclass(_qt_lineEdit)::textChanged,
	     this,         &pclass(this)::changed );

    if ( passwordMode )
    {
	_qt_lineEdit->setEchoMode( QLineEdit::Password );

	connect( _qt_lineEdit,	&pclass(_qt_lineEdit)::capsLockActivated,
		 this,		&pclass(this)::displayCapsLockWarning );

	connect( _qt_lineEdit,	&pclass(_qt_lineEdit)::capsLockDeactivated,
		 this,		&pclass(this)::clearCapsLockWarning );
    }
}


string YQInputField::value()
{
    return toUTF8( _qt_lineEdit->text() );
}


void YQInputField::setValue( const string & newText )
{
    QString text = fromUTF8( newText );

    if ( isValidText( text ) )
    {
	YQSignalBlocker sigBlocker( _qt_lineEdit );
	_qt_lineEdit->setText( text );
    }
    else
    {
	yuiError() << this << ": Rejecting invalid value \"" << newText << "\"" << endl;
    }
}


void YQInputField::setEnabled( bool enabled )
{
    _qt_lineEdit->setEnabled( enabled );
    _caption->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQInputField::preferredWidth()
{
    int minSize	  = shrinkable() ? 30 : 200;
    int hintWidth = !_caption->isHidden()
	? _caption->sizeHint().width() + 2 * YQWidgetMargin
	: 0;

    return std::max( minSize, hintWidth );
}


int YQInputField::preferredHeight()
{
    return sizeHint().height();
}


void YQInputField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQInputField::setLabel( const string & label )
{
    _caption->setText( label );
    YInputField::setLabel( label );
}


bool YQInputField::isValidText( const QString & txt ) const
{
    if ( ! _validator )
	return true;

    int pos = 0;
    QString text( txt );	// need a non-const QString &

    return _validator->validate( text, pos ) == QValidator::Acceptable;
}


void YQInputField::setValidChars( const string & newValidChars )
{
    if ( _validator )
    {
	_validator->setValidChars( fromUTF8( newValidChars ) );
    }
    else
    {
	_validator = new QY2CharValidator( fromUTF8( newValidChars ), this );
	_qt_lineEdit->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    if ( ! isValidText( _qt_lineEdit->text() ) )
    {
	yuiError() << this << ": Old value \"" << _qt_lineEdit->text()
		   << "\" invalid according to new ValidChars \"" << newValidChars
		   << "\" - deleting"
		   << endl;

	_qt_lineEdit->setText( "" );
    }

    YInputField::setValidChars( newValidChars );
}

void YQInputField::setInputMaxLength( int len )
{
    _qt_lineEdit->setMaxLength( len );
    YInputField::setInputMaxLength( len );
}

bool YQInputField::setKeyboardFocus()
{
    _qt_lineEdit->setFocus();
    _qt_lineEdit->selectAll();

    return true;
}


void YQInputField::changed( const QString & )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQInputField::displayCapsLockWarning()
{
    yuiMilestone() << "warning" << endl;
    if ( _displayingCapsLockWarning )
	return;

    if ( _qt_lineEdit->echoMode() == QLineEdit::Normal )
	return;

    // Translators: This is a very short warning that the CapsLock key
    // is active while trying to type in a password field. This warning
    // replaces the normal label (caption) of that password field while
    // CapsLock is active, so please keep it short. Please don't translate it
    // at all if the term "CapsLock" can reasonably expected to be understood
    // by the target audience.
    //
    // In particular, please don't translate this to death in German.
    // Simply leave it.

    _caption->setText( _( "CapsLock!" ) );
    _displayingCapsLockWarning = true;
}


void YQInputField::clearCapsLockWarning()
{
    yuiMilestone() << "warning off " << endl;
    if ( ! _displayingCapsLockWarning )
	return;

    if ( _qt_lineEdit->echoMode() == QLineEdit::Normal )
	return;

    _caption->setText( label() );
    _displayingCapsLockWarning = false;
}


#if 0
bool YQRawLineEdit::x11Event( XEvent * event )
{
    // FIXME: This used to check CapsLock, but it silently stopped working from
    //        Qt 5.x on. Nobody ever realized this, but we want this feature
    //        back some day soon.
    //
    // The reason why this never failed to compile was that this is overwriting
    // a virtual function from the QWidget parent class, but the parent class
    // simply doesn't have that function anymore, so this is just implementing
    // a new function that is never called; so this failed silently.
    //
    // -- 2020-02-25 shundhammer


    // Qt (3.x) does not have support for the CapsLock key.
    // All other modifiers (Shift, Control, Meta) are propagated via
    // Qt's events, but for some reason, CapsLock is not.
    //
    // So let's examine the raw X11 event here to check for the
    // CapsLock status. All events are really handled on the parent class
    // (QWidget) level, though. We only peek into the modifier states.

    if ( event )
    {
	bool oldCapsLockActive = _capsLockActive;

	switch ( event->type )
	{
	    case KeyPress:
		_capsLockActive = (bool) ( event->xkey.state & LockMask );
		break;

	    case KeyRelease:

		_capsLockActive = (bool) ( event->xkey.state & LockMask );

		if ( _capsLockActive && oldCapsLockActive )
		{
		    KeySym key = XLookupKeysym( &(event->xkey), 0 );

		    if ( key == XK_Caps_Lock ||
			 key == XK_Shift_Lock  )
		    {
			yuiMilestone() << "CapsLock released" << endl;
			_capsLockActive = false;
		    }
		}

		if ( _capsLockActive )
		    yuiDebug() << "Key event; caps lock: "
			       << std::boolalpha << _capsLockActive << std::noboolalpha
			       << endl;
		break;

	    case ButtonPress:
	    case ButtonRelease:
		_capsLockActive = (bool) ( event->xbutton.state & LockMask );
		break;

	    case EnterNotify:
		_capsLockActive = (bool) ( event->xcrossing.state & LockMask );
		break;

	    case LeaveNotify:
	    case FocusOut:
		_capsLockActive = false;
		emit capsLockDeactivated();
		break;

	    default:
		break;
	}

	if ( oldCapsLockActive != _capsLockActive )
	{
	    yuiMilestone() << "Emitting warning" << endl;

	    if ( _capsLockActive )
		emit capsLockActivated();
	    else
		emit capsLockDeactivated();
	}
    }

    return false; // handle this event at the Qt level
}
#endif
