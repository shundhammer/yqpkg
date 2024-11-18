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

  File:	      YQSquash.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include "YQSquash.h"


YQSquash::YQSquash( YWidget *	parent,
		    bool 	horSquash,
		    bool 	vertSquash )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YSquash( parent, horSquash, vertSquash )
{
    setWidgetRep( this );
}


YQSquash::~YQSquash()
{
    // NOP
}


void YQSquash::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void YQSquash::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YSquash::setSize( newWidth, newHeight );
}



