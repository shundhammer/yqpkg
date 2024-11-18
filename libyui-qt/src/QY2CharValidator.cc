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

  File:	      QY2CharValidator.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt object - it can be used independently of YaST2.

/-*/


#include <stdio.h>
#include <QValidator>
#include "QY2CharValidator.h"



QY2CharValidator::QY2CharValidator( const QString &	initialValidChars,
				    QObject *		parent,
				    const char *	name )
    : QValidator( parent )
    , _validChars( initialValidChars )
{
  setObjectName(name);
}


QY2CharValidator::~QY2CharValidator()
{
    // NOP
}


QValidator::State
QY2CharValidator::validate( QString & fieldContents, int & pos ) const
{
    if ( validChars().isEmpty() || fieldContents.isEmpty() )
	return QValidator::Acceptable;


    // Check the entire field contents.
    //
    // There might be more than one new character - the user might have copied
    // some longer text via the X clipboard.

    for ( int i=0; i < fieldContents.length(); i++ )
    {
	if ( ! validChars().contains( fieldContents[i] ) )
	    return QValidator::Invalid;
    }

    return QValidator::Acceptable;
}



