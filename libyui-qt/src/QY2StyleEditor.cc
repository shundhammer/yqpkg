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

  File:		QY2StyleEditor.cc

  Author:	Thomas Goettlicher <tgoettlicher@suse.de>

  Textdomain	"qt"

/-*/

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextObject>
#include <QDebug>

#include "YQUI.h"
#include "QY2StyleEditor.h"
#include "QY2Styler.h"
#include "YQi18n.h"


QY2StyleEditor::QY2StyleEditor( QWidget *parent )
    : QDialog( parent )
{
    ui.setupUi(this);

    ui.textEdit->setPlainText( "/* enter style sheet here  */" );
    ui.closeButton->setAutoDefault(false);
    setWindowTitle( _( "Stylesheet Editor" ) );

    connect( ui.applyButton, &pclass(ui.applyButton)::clicked,
             this, &pclass(this)::slotApplyStyle);

    connect( ui.closeButton, &pclass(ui.closeButton)::clicked,
             this, &pclass(this)::close);

    connect( ui.loadButton, &pclass(ui.loadButton)::clicked,
             this, &pclass(this)::slotLoadFile);

    connect( ui.textEdit, &pclass(ui.textEdit)::textChanged,
             this, &pclass(this)::slotTextChanged);

    connect( ui.autoApply, &pclass(ui.autoApply)::stateChanged,
             this, &pclass(this)::slotTextChanged);
}


QY2StyleEditor::~QY2StyleEditor()
{
}

void QY2StyleEditor::slotTextChanged()
{
    if ( ui.autoApply->isChecked() )
        slotApplyStyle();
}

void QY2StyleEditor::slotApplyStyle()
{
    QY2Styler::styler()->setStyleSheet( ui.textEdit->toPlainText() );
}


void QY2StyleEditor::slotLoadFile()
{

    QString fileName = QFileDialog::getOpenFileName( this,      // parent
                                                    QString( "Load stylesheet ..." ), // caption
                                                    QY2Styler::styler()->themeDir(), // dir
                                                    QString( "*.qss"), 0,            // filter
                                                    QFileDialog::DontUseNativeDialog );

    if ( fileName.isEmpty() )
        return;         // user clicked cancel


    QFile file( fileName);

    if ( file.open( QFile::ReadOnly ) )
        ui.textEdit->setPlainText( file.readAll() );
    else
    {
        QMessageBox::warning( this,                     // parent
            QString("Error") ,                          // caption
            QString( "Couldn't load file\n%1" ).arg( fileName ),
            QMessageBox::Ok | QMessageBox::Default,     // button0
            Qt::NoButton,                               // button1
            Qt::NoButton );                             // button2
    }

}




