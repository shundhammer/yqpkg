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

  File:	      YQProgressBar.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qprogressbar.h>
#include <QVBoxLayout>

#include <qlabel.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQProgressBar.h"
#include "YQWidgetCaption.h"

using std::string;


YQProgressBar::YQProgressBar( YWidget * 	parent,
			      const string &	label,
			      int		maxValue )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YProgressBar( parent, label, maxValue )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_progressbar = new QProgressBar( this );
    _qt_progressbar->setRange(0, maxValue);
    YUI_CHECK_NEW( _qt_progressbar );
    layout->addWidget( _qt_progressbar );

    _caption->setBuddy( _qt_progressbar );
}


YQProgressBar::~YQProgressBar()
{
    // NOP
}


void YQProgressBar::setLabel( const string & label )
{
    _caption->setText( label );
    YProgressBar::setLabel( label );
}


void YQProgressBar::setValue( int newValue )
{
    YProgressBar::setValue( newValue );
    _qt_progressbar->setValue( value() );
}



void YQProgressBar::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_progressbar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQProgressBar::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ?
        _caption->sizeHint().width() + layout()->margin() : 0;

    return std::max( 200, hintWidth );
}


int YQProgressBar::preferredHeight()
{
    return sizeHint().height();
}


void YQProgressBar::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQProgressBar::setKeyboardFocus()
{
    _qt_progressbar->setFocus();

    return true;
}



