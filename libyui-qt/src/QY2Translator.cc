/*
  Copyright (c) 2022 SUSE LLC
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


#include "QY2Translator.h"
#include "YQi18n.h"
#include "utf8.h"

#define TEXTDOMAIN "qt"


QY2Translator::QY2Translator( QObject * parent )
    : QTranslator( parent )
{
    
}


QY2Translator::~QY2Translator()
{
    // NOP
}


QString
QY2Translator::translate( const char * context_str,
                          const char * sourceText,
                          const char * disambiguation,
                          int          nPlural         ) const
{
    QString context( fromUTF8( context_str ) );
                    
    if ( context.startsWith( "QY2" ) ||
         context.startsWith( "YQ"  )   )
    {
         // Use GNU gettext from YQi18n.h
        
        return _( sourceText );
    }
    else
    {
        // Fallback: Use Qt's built-in translation method
        // (many predefined Qt dialogs and (context) menus use that)
        
        return QTranslator::translate( context_str,
                                       sourceText,
                                       disambiguation,
                                       nPlural );
    }
}
    
