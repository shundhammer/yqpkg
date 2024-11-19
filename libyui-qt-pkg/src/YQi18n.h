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

  File:		YQi18n.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQi18n_h
#define YQi18n_h

#include <libintl.h>
#include <qstring.h>


inline QString _( const char * msgid )
{
	return ( !msgid || !*msgid ) ? "" : QString::fromUtf8( gettext(msgid ) );
}

inline QString _( const char * msgid1, const char * msgid2, unsigned long int n )
{
	return QString::fromUtf8( ngettext(msgid1, msgid2, n ) );
}


#endif // YQi18n_h
