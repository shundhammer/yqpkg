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

  File:	      QY2StyleSheetSelector.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt dialog without any YQWidgets or YWidgets.

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "QY2StyleSheetSelector.h"
#include "ui_QY2StyleSheetSelector.h"
#include "QY2Styler.h"
#include "YQSignalBlocker.h"

#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "qt"


QY2StyleSheetSelector::QY2StyleSheetSelector( QWidget * parent )
    : QDialog( parent )
    , _ui( new Ui_QY2StyleSheetSelector() )
{
    _ui->setupUi( this );
    initWidgets();
    populate();
}


QY2StyleSheetSelector::~QY2StyleSheetSelector()
{
    delete _ui;
}


void
QY2StyleSheetSelector::initWidgets()
{
    // The "Close" button is already connected to the 'accept()' slot
    // in the .ui file.

    connect( _ui->resetButton,  SIGNAL( clicked() ),
             this,              SLOT  ( resetToDefault() ) );

    connect( _ui->stylesList,   SIGNAL( itemSelectionChanged() ),
             this,              SLOT  ( applySelectedStyle()   ) );
}


void
QY2StyleSheetSelector::populate()
{
    YQSignalBlocker sigBlocker( _ui->stylesList );

    // Prevent the _ui->stylesList from sending any signals, in particular the
    // selectionChanged() signal: Otherwise the currently already selected
    // style sheet will be loaded again, and if the user did any changes to it
    // with the style sheet editor, the mere act of opening this style sheet
    // selector dialog will revert those changes when the style sheet is loaded
    // again from file.
    //
    // Without that signal, there is no change unless he really clicks on any
    // style sheet.


    foreach ( const QString & styleSheet, QY2Styler::styler()->allStyleSheets() )
    {
        QListWidgetItem * item = new QListWidgetItem( styleSheet );
        _ui->stylesList->addItem( item );

        if ( styleSheet == QY2Styler::styler()->currentStyleSheet() )
        {
            item->setSelected( true );
            _ui->stylesList->setCurrentItem( item );
        }
    }
}


void
QY2StyleSheetSelector::resetToDefault()
{
    QString defaultStyle = QY2Styler::styler()->defaultStyleSheet();
    QList<QListWidgetItem *> items = _ui->stylesList->findItems( defaultStyle, Qt::MatchExactly );

    if ( ! items.isEmpty() )
    {
        items.first()->setSelected( true );

        // This implicitly deselects any previously selected item since we are
        // using the default single selection policy; and it will also trigger
        // the connected applySelectedStyle() slot and load that style sheet.
    }
}


void
QY2StyleSheetSelector::applySelectedStyle()
{
    QList<QListWidgetItem *> sel = _ui->stylesList->selectedItems();

    if ( ! sel.isEmpty() )
    {
        QString selectedStyle = sel.first()->text();
        QY2Styler::styler()->loadStyleSheet( selectedStyle );
    }
}

