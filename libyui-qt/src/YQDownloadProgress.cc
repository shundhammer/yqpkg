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

  File:	      YQLogView.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qlabel.h>
#include <qtimer.h>
#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQDownloadProgress.h"
#include "YQWidgetCaption.h"
#include <QVBoxLayout>
#include <QProgressBar>

using std::string;


YQDownloadProgress::YQDownloadProgress( YWidget *	parent,
					const string & 	label,
					const string &	filename,
					YFileSize_t	expectedSize )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YDownloadProgress( parent, label, filename, expectedSize )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setMargin( YQWidgetMargin );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_progressBar = new QProgressBar( this );
    YUI_CHECK_NEW( _qt_progressBar );
    layout->addWidget( _qt_progressBar );

    _qt_progressBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    _qt_progressBar->setRange( 0, 100 ); // Using percent
    _qt_progressBar->setValue( currentPercent() );

    _timer = new QTimer( this );

    connect( _timer, 	&pclass(_timer)::timeout,
	     this,	&pclass(this)::pollFileSize );

    _timer->setSingleShot(false);
    _timer->start( 250 );// millisec
}


YQDownloadProgress::~YQDownloadProgress()
{
    // NOP
}


void
YQDownloadProgress::setLabel( const string & label )
{
    _caption->setText( label );
    YDownloadProgress::setLabel( label );
}


void
YQDownloadProgress::setFilename( const string & filename )
{
    YDownloadProgress::setFilename( filename );
    _qt_progressBar->setValue( currentPercent() );
}


void
YQDownloadProgress::setExpectedSize( YFileSize_t expectedSize )
{
    _qt_progressBar->setValue( currentPercent() );
    YDownloadProgress::setExpectedSize( expectedSize );
}


void
YQDownloadProgress::pollFileSize()
{
    _qt_progressBar->setValue( currentPercent() );
}


void
YQDownloadProgress::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_progressBar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQDownloadProgress::preferredWidth()
{
    return sizeHint().width();
}


int
YQDownloadProgress::preferredHeight()
{
    return sizeHint().height();
}


void
YQDownloadProgress::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}

