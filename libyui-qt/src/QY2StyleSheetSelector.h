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


/*-/

  File:	      QY2StyleSheetSelector.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt dialog without any YQWidgets or YWidgets.

/-*/


#ifndef QY2StyleSheetSelector_h
#define QY2StyleSheetSelector_h


#include <QDialog>
#include "ui_QY2StyleSheetSelector.h"


class Ui_QY2StyleSheetSelector;


/**
 * This is a dialog that lets the user select a different .qss widget style
 * sheet on the fly from all the .qss files in the theme directory.
 *
 * Upon selecting a style sheet from the list, it is applied instantly. But
 * there is a "Reset to Defaults" button.
 **/
class QY2StyleSheetSelector: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    QY2StyleSheetSelector( QWidget * parent = 0 );

    /**
     * Destructor.
     **/
    virtual ~QY2StyleSheetSelector();


public slots:

    /**
     * Reset the dialog to default, i.e. reload the default style.
     **/
    void resetToDefault();

    /**
     * Apply the currently selected style.
     **/
    void applySelectedStyle();


protected:

    void initWidgets();
    void populate();

    // Data members

    Ui_QY2StyleSheetSelector * _ui;

};      // class QY2StyleSheetSelector


#endif  // QY2StyleSheetSelector_h
