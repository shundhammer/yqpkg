/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */



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
