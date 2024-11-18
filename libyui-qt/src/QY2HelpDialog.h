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

  File:       QY2HelpDialog.h

  Author:     Stephan Kulow <coolo@suse.de>

  Textdomain    "qt"

/-*/

#ifndef _QY2HelpDialog_h
#define _QY2HelpDialog_h

#include <QDialog>
#include <QColor>

class QTextCursor;
class Ui_QY2HelpDialog;

class QY2HelpDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QColor searchResultForeground READ getSearchResultForeground WRITE setSearchResultForeground DESIGNABLE true)
    Q_PROPERTY(QColor searchResultBackground READ getSearchResultBackground WRITE setSearchResultBackground DESIGNABLE true)

public:
    QY2HelpDialog( const QString &helpText, QWidget *parent );
    ~QY2HelpDialog();
    void setHelpText( const QString &helpText );
    void retranslate();
    QColor getSearchResultForeground();
    QColor getSearchResultBackground();
    void setSearchResultForeground( QColor pen );
    void setSearchResultBackground( QColor pen );

public slots:
    void searchStringChanged( QString );

private:
    Ui_QY2HelpDialog *_ui;
    QList<QTextCursor> _marks;
    QColor _searchResultForeground;
    QColor _searchResultBackground;


};

#endif

