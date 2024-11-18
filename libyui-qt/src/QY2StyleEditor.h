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

  File:         QY2StyleEditor.h

  Author:       Thomas Goettlicher <tgoettlicher@suse.de>

  Textdomain    "qt"

/-*/

#ifndef _QY2StyleEditor_h
#define _QY2StyleEditor_h

#include <QDialog>
#include "ui_QY2StyleEditor.h"


/**
 * @short Stylesheet Editor Dialog
 **/

class QY2StyleEditor : public QDialog
{
    Q_OBJECT

public:

    /**
    * Constructor.
    **/
    QY2StyleEditor( QWidget *parent );

    /**
    * Destructor.
    **/
    ~QY2StyleEditor();

private slots:

    /**
    * sets the text written in the text browser as style sheet
    **/
    void slotApplyStyle();

    /**
    * loads a style sheet form a file
    **/
    void slotLoadFile();

    /**
    * triggers style sheet auto apply if user has selected
    **/
    void slotTextChanged();

private:
     Ui_QY2StyleEditor ui;

};

#endif

