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


#ifndef QY2Translator_h
#define QY2Translator_h

#include <QTranslator>
#include "YQi18n.h"

/**
 * Helper class to enable YaST translations (using xgettext) even in a Qt
 * context, e.g. for .ui files which use QCoreApplication::translate() in the
 * generated ui_*.h files.
 *
 * NOTICE: This requires a QY2 or YQ prefix for those classes!
 *
 * This is not commonly used for other translations in the YaST / libyui-qt
 * context, just for .ui files (created with Qt Designer).
 **/
class QY2Translator: public QTranslator
{
    Q_OBJECT

public:

    QY2Translator( QObject * parent );
    virtual ~QY2Translator();

    /**
     * Reimplemented from QTranslator:
     *
     * Use GNU gettext like everywhere in YaST if 'context' (usually the class
     * name) starts with "QY2" or "YQ", fall back to Qt's built-in translation
     * method otherwise.
     *
     * Notice that this is also used for Qt's own messages that need to be
     * translated, such as predefined dialogs (file dialog) and context menus
     * (e.g. when right-clicking input fields, lists, scroll bars), so we
     * cannot simply use GNU gettext everywhere.
     **/
    virtual QString translate( const char * context,
                               const char * sourceText,
                               const char * disambiguation = 0,
                               int          nPlural        = -1 ) const Q_DECL_OVERRIDE;

};      // class QY2Translator

#endif  // QY2Translator_h
