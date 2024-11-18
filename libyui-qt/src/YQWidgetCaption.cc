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

  File:	      YQWidgetCaption.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "YQWidgetCaption.h"
#include "utf8.h"

using std::string;



YQWidgetCaption::YQWidgetCaption( QWidget *       parent,
				  const string &  text )
    : QLabel( fromUTF8( text ), parent )
{
    setTextFormat( Qt::PlainText );
    handleVisibility( text );
}


YQWidgetCaption::YQWidgetCaption( QWidget * parent, const QString & text )
    : QLabel( text, parent )
{
    setTextFormat( Qt::PlainText );
    handleVisibility( text );
}


YQWidgetCaption::~YQWidgetCaption()
{
    // NOP
}


void YQWidgetCaption::setText ( const string  & newText )
{
    setText( fromUTF8( newText ) );
}


void YQWidgetCaption::setText ( const QString & newText )
{
    QLabel::setText( newText );
    handleVisibility( newText.isEmpty() );
}


void YQWidgetCaption::handleVisibility( const string  & text )
{
    handleVisibility( text.empty() );
}


void YQWidgetCaption::handleVisibility( const QString & text )
{
    handleVisibility( text.isEmpty() );
}


void YQWidgetCaption::handleVisibility( bool textIsEmpty )
{
    if ( textIsEmpty )
    {
	if ( !isHidden() )
	    hide();
    }
    else
    {
	if ( isHidden() )
	    show();
    }
}
