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

  File:	      YQBusyIndicator.cc

  Author:     Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


//#include <qprogressbar.h>
#include <QLabel>
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>
#include <QFrame>
#include <QPainter>
#include <math.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQBusyIndicator.h"
#include "YQWidgetCaption.h"


#define REPAINT_INTERVAL	100
#define STEP_SIZE 		.05
#define MINIMUM_WITDH		100
#define MINIMUM_HEIGHT		24

using std::string;



BusyBar::BusyBar(QWidget *parent)
	: QFrame(parent)
	, _position(.5)
	, _rightwards(true)
	, _alive(true)
{
    setMinimumSize(MINIMUM_WITDH, MINIMUM_HEIGHT);

    _timer = new QTimer(this);
    connect(_timer, &pclass(_timer)::timeout, this, &pclass(this)::update);
    _timer->start(REPAINT_INTERVAL);

    setFrameStyle (QFrame::Panel |  QFrame::Sunken );
    setLineWidth(2);
    setMidLineWidth(2);
}

void BusyBar::update()
{
    if (!_alive)
	return;

    if (_position > 1.0 - STEP_SIZE || _position < STEP_SIZE )
	_rightwards = !_rightwards;

    if (_rightwards)
	_position += STEP_SIZE;
    else
	_position -= STEP_SIZE;

    repaint();
}

void BusyBar::run()
{
    _alive=true;
}

void BusyBar::stop()
{
    _alive=false;
}

void BusyBar::paintEvent( QPaintEvent * e )
{

    QPalette palette = QApplication::palette();
    QColor foreground = palette.color( QPalette::Active, QPalette::Highlight );
    QColor background = palette.color( QPalette::Active, QPalette::Base );

    QPainter painter(this);
    QLinearGradient gradient(0, 0, width()-1, 0 );

    gradient.setColorAt( 0.0, background );
    gradient.setColorAt( _position, foreground );
    gradient.setColorAt( 1.0, background );

    painter.setBrush( gradient );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect() );
    painter.end();

    QFrame::paintEvent( e );
}


YQBusyIndicator::YQBusyIndicator( YWidget * 	parent,
			      const string &	label,
			      int		timeout )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YBusyIndicator( parent, label, timeout )
    , _timeout (timeout)
{

    _timer = new QTimer(this);
    connect(_timer, &pclass(_timer)::timeout, this, &pclass(this)::setStalled);
    _timer->start(_timeout);

    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _bar = new BusyBar( this );
    YUI_CHECK_NEW ( _bar );
    layout->addWidget( _bar );
    _caption->setBuddy( _bar );

}


YQBusyIndicator::~YQBusyIndicator()
{
    // NOP
}


void YQBusyIndicator::setLabel( const string & label )
{
    _caption->setText( label );
    YBusyIndicator::setLabel( label );
}


void YQBusyIndicator::setAlive( bool newAlive )
{
    YBusyIndicator::setAlive( newAlive );
    if (newAlive)
    {
	_bar->run();
	_timer->stop();
	_timer->start(_timeout);
    }
    else
    {
	_bar->stop();
	_timer->stop();
    }
}


void YQBusyIndicator::setStalled()
{
    setAlive( false );
}


void YQBusyIndicator::setTimeout( int newTimeout )
{
    _timeout = newTimeout;
    YBusyIndicator::setTimeout( newTimeout );
}


void YQBusyIndicator::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _bar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQBusyIndicator::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ?
      _caption->sizeHint().width() + layout()->margin() : 0;

    return std::max( 200, hintWidth );
}


int YQBusyIndicator::preferredHeight()
{
    return sizeHint().height();
}


void YQBusyIndicator::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQBusyIndicator::setKeyboardFocus()
{
    _bar->setFocus();

    return true;
}



