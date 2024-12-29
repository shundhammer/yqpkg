/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "ProgressDialog.h"


ProgressDialog::ProgressDialog( const QString & text, QWidget * parent )
    : QDialog( parent ? parent : MainWindow::instance() )
    , _ui( new Ui::ProgressDialog )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );
    setText( text );
    reset();
}


ProgressDialog::~ProgressDialog()
{
    // NOP
}


void ProgressDialog::reset()
{
    _ui->progressBar->setValue( 0 );
}
