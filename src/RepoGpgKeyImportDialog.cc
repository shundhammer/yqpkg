/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QMessageBox>

#include "Exception.h"
#include "Logger.h"
#include "MainWindow.h"
#include "utf8.h"
#include "YQi18n.h"
#include "RepoGpgKeyImportDialog.h"


RepoGpgKeyImportDialog::RepoGpgKeyImportDialog( const zypp::PublicKey & key,
                                                const ZyppRepoInfo  &   repoInfo,
                                                QWidget *               parent   )
    : QDialog( parent ? parent : MainWindow::instance() )
    , _ui( new Ui::RepoGpgKeyImportDialog ) // Use the Qt designer .ui form (XML)
    , _repoInfo( repoInfo )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this ); // Actually create the widgets from the .ui form


    // See ui_repo-gpg-key-import-dialog.h for the widget names.
    //
    // That header is generated by Qt's uic (user interface compiler)
    // from the XML .ui file created with Qt designer.
    //
    // Take care in Qt designer to give each widget a meaningful name in the
    // widget tree at the top right: They are also the member variable names
    // for the _ui object.

    setText( _ui->repoName,    repoInfo.name() );
    setText( _ui->repoUrl,     repoInfo.url().asString() );
    setText( _ui->fingerprint, zypp::str::gapify( key.fingerprint(), 4 ) );
    setText( _ui->keyName,     key.name()     );
    setText( _ui->keyAlgo,     key.algoName() );
    setText( _ui->keyId,       key.id()       );
    setText( _ui->keyCreated,  key.created().form( "%F" ) ); // ISO date ("2025-01-15"); see 'man strftime'
    setText( _ui->keyExpires,  key.expires().form( "%F" ) );

    qApp->setOverrideCursor( Qt::ArrowCursor );

    connect( _ui->trustButton, SIGNAL ( clicked() ),
             this,             SLOT   ( accept()  ) );

    connect( _ui->cancelButton, SIGNAL( clicked() ),
             this,              SLOT  ( reject()  ) );

    logDebug() << "Asking user to accept the GPG key for repo " << _ui->repoName->text() << endl;
}


RepoGpgKeyImportDialog::~RepoGpgKeyImportDialog()
{
    delete _ui;
}


void RepoGpgKeyImportDialog::accept()
{
    logResult( "Accepting" );

    qApp->restoreOverrideCursor();
    QDialog::accept();
}


void RepoGpgKeyImportDialog::reject()
{
    logResult( "Rejecting" );
    disableRepo();

    qApp->restoreOverrideCursor();
    QDialog::reject();
}


void RepoGpgKeyImportDialog::logResult( const QString & result )
{
    logInfo() << result << " GPG key for repo " << _ui->repoName->text() << endl;
    logInfo() << "URL:         " << _ui->repoUrl->text()     << endl;
    logInfo() << "Fingerprint: " << _ui->fingerprint->text() << endl;
    logInfo() << "Key name:    " << _ui->keyName->text()     << endl;
}


void RepoGpgKeyImportDialog::setText( QLabel * label, const std::string & text )
{
    label->setText( fromUTF8( text ) );
}


void RepoGpgKeyImportDialog::disableRepo()
{
    QString msg = _( "Repository \"%1\" will be temporarily disabled." ).arg( fromUTF8( _repoInfo.name()  ) );

    QMessageBox msgBox( this );
    msgBox.setText( msg );
    msgBox.setIcon( QMessageBox::Warning );
    msgBox.addButton( QMessageBox::Ok );
    msgBox.exec();

    _repoInfo.setEnabled( false );
}
