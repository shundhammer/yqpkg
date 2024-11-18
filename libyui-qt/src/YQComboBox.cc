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

  File:	      YQComboBox.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define SEND_SELECTION_CHANGED_EVENT 0

#include <qstring.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include <yui/YEvent.h>
#include "QY2CharValidator.h"
#include "YQComboBox.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>
#include <QDebug>

using std::string;
using std::endl;



YQComboBox::YQComboBox( YWidget * 	parent,
			const string &	label,
			bool		editable )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YComboBox( parent, label, editable )
    , _validator(0)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_comboBox = new QComboBox(this);
    _qt_comboBox->setEditable(editable);
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _qt_comboBox );

    _caption->setBuddy( _qt_comboBox );

#if SEND_SELECTION_CHANGED_EVENT
    connect( _qt_comboBox,	&pclass(_qt_comboBox)::highlighted,
	     this,		&pclass(this)::slotSelected );
#endif

#if (QT_VERSION < QT_VERSION_CHECK( 5, 15, 0 ))

    // The Trolls introduced a bunch of SOURCE CODE INCOMPATIBILITIES in a MINOR RELEASE!
    // With Qt 5.15, they deprecated a ton of functions, causing a ton of warnings.
    // For us, this means a build failure because we always compile with -Werror,
    // promoting all warnings to errors.
    //
    // Seriously, Trolls, WTF?!?
    //
    // And now we have to live with ugly stuff like this #if QT_VERSION. Thanks a lot.
    //
    // -- 2020-05-20 HuHa

    connect( _qt_comboBox,	static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated),
	     this,		&pclass(this)::textChanged );
#else
    connect( _qt_comboBox,	static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::textActivated),
	     this,		&pclass(this)::textChanged );
#endif

    connect( _qt_comboBox,	&pclass(_qt_comboBox)::editTextChanged,
	     this,		&pclass(this)::textChanged );
}


YQComboBox::~YQComboBox()
{
    // NOP
}


string YQComboBox::text()
{
    return toUTF8( _qt_comboBox->currentText() );
}


void YQComboBox::setText( const string & newValue )
{
    QString text = fromUTF8( newValue );

    if ( isValidText( text ) )
    {
	YQSignalBlocker sigBlocker( _qt_comboBox );
        int index = _qt_comboBox->findText( text );
        if ( index < 0 )
            _qt_comboBox->setEditText( text );
        else
        {
            _qt_comboBox->setCurrentIndex( index );
            _qt_comboBox->setItemText(index, text );
        }
    }
    else
    {
	yuiError() << this << ": Rejecting invalid value \"" << newValue << "\"" << endl;
    }
}


void YQComboBox::addItem( YItem * item )
{
    YComboBox::addItem( item );
    QIcon icon;

    if ( item->hasIconName() )
    {
        icon = YQUI::ui()->loadIcon( item->iconName() );
    }

    if ( icon.isNull() )
	_qt_comboBox->addItem( fromUTF8( item->label() ) );
    else
	_qt_comboBox->addItem( icon, fromUTF8( item->label() ) );

    if ( item->selected() )
    {
	YQSignalBlocker sigBlocker( _qt_comboBox );
	setText( item->label() );
    }
}


void YQComboBox::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_comboBox );

    _qt_comboBox->clear();
    YComboBox::deleteAllItems();
}


void YQComboBox::setLabel( const string & label )
{
    _caption->setText( label );
    YComboBox::setLabel( label );
}


void YQComboBox::setValidChars( const string & newValidChars )
{
    if ( ! _qt_comboBox->isEditable() )
    {
	yuiWarning() << this << ": Setting ValidChars is useless on a combo box that isn't editable!" << endl;
	return;
    }

    if ( _validator )
    {
	_validator->setValidChars( fromUTF8( newValidChars ) );
    }
    else
    {
	_validator = new QY2CharValidator( fromUTF8( newValidChars ), this );
	_qt_comboBox->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    if ( ! isValidText( _qt_comboBox->currentText() ) )
    {
	yuiError() << this << ": Old value \"" << _qt_comboBox->currentText()
		   << " \" invalid according to new ValidChars \""<< newValidChars << "\" - deleting"
		   << endl;
	_qt_comboBox->setItemText(_qt_comboBox->currentIndex(), "");
    }

    YComboBox::setValidChars( newValidChars );
}


bool YQComboBox::isValidText( const QString & txt ) const
{
    if ( ! _validator )
	return true;

    int pos = 0;
    QString text( txt );	// need a non-const QString &

    return _validator->validate( text, pos ) == QValidator::Acceptable;
}


void YQComboBox::slotSelected( int i )
{
    if ( notify() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) ValueChanged event with a SelectionChanged event

	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void YQComboBox::textChanged( QString )
{
    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQComboBox::setInputMaxLength( int len )
{
    _qt_comboBox->lineEdit()->setMaxLength( len );
    YComboBox::setInputMaxLength( len );
}


int YQComboBox::preferredWidth()
{
    return sizeHint().width();
}


int YQComboBox::preferredHeight()
{
    return sizeHint().height();
}


void YQComboBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQComboBox::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_comboBox->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQComboBox::setKeyboardFocus()
{
    _qt_comboBox->setFocus();

    return true;
}

