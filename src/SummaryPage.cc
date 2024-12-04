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


#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QSettings>

#include "Logger.h"
#include "Exception.h"
#include "MainWindow.h"
#include "YQi18n.h"
#include "SummaryPage.h"


SummaryPage::SummaryPage( QWidget * parent )
    : QWidget( parent )
    , _ui( new Ui::SummaryPage ) // Use the Qt designer .ui form (XML)
    , _countdownSec( 30 )
    , _countdownMenu( 0 )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this ); // Actually create the widgets from the .ui form

    // See ui_summary-page.h for the widget names.
    //
    // That header is generated by Qt's uic (user interface compiler)
    // from the XML .ui file created with Qt designer.
    //
    // Take care in Qt designer to give each widget a meaningful name in the
    // widget tree at the top right: They are also the member variable names
    // for the _ui object.

    readSettings();
    setupCountdownMenuButton();
    reset();
    connectWidgets();

    // FIXME: TO DO: countdownMenuButton menu and handling


    // Notice that startCountdown() needs to be called from the outside.
}


SummaryPage::~SummaryPage()
{
    logDebug() << "Destroying SummaryPage..." << endl;
    writeSettings();

    if ( _countdownMenu )
        delete _countdownMenu;

    delete _ui;
    logDebug() << "Destroying SummaryPage done" << endl;
}


void SummaryPage::connectWidgets()
{
    connect( &_countdownTimer,  SIGNAL( timeout() ),
             this,              SLOT  ( timeout() ) );

    connect( &_intervalTimer,   SIGNAL( timeout() ),
             this,              SLOT  ( updateCountdownWidgets() ) );

    connect( _ui->stopButton,   SIGNAL( clicked()       ),
             this,              SLOT  ( stopCountdown() ) );

    connect( _ui->backButton,   SIGNAL( clicked() ),
             this,              SIGNAL( back()    ) );

    connect( _ui->finishButton, SIGNAL( clicked() ),
             this,              SIGNAL( finish()  ) );
}



void SummaryPage::reset()
{
    stopCountdown(); // This also updates the countdown widgets
    _ui->contentTextEdit->clear();
}


void SummaryPage::updateSummary()
{
    QString text = _( "No package changes." );

    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO

    _ui->contentTextEdit->setText( text );
}


void SummaryPage::updateCountdownWidgets()
{
    _ui->countdownCaption->setVisible( _countdownSec > 0 );
    _ui->countdownLabel->setVisible  ( _countdownSec > 0 );
    _ui->stopButton->setVisible      ( _countdownSec > 0 );

    bool timerActive = _countdownTimer.isActive();

    _ui->countdownCaption->setEnabled( timerActive );
    _ui->countdownLabel->setEnabled  ( timerActive );
    _ui->stopButton->setEnabled      ( timerActive );

    QString remaining( "---" );

    if ( timerActive )
    {
        float seconds = _countdownTimer.remainingTime() / 1000.0 ;

        // Translators: Number of seconds remaining in a countdown
        remaining = _( "%1 s" ).arg( (int) (seconds + 0.5) );
    }

    _ui->countdownLabel->setText( remaining );
}


void SummaryPage::startCountdown()
{
    if ( _countdownSec > 0 )
    {
        _intervalTimer.setInterval( 1000 ); // millisec
        _intervalTimer.start();

        _countdownTimer.setSingleShot( true );
        _countdownTimer.start( _countdownSec * 1000 );
    }

    updateCountdownWidgets();
}


void SummaryPage::stopCountdown()
{
    _countdownTimer.stop();
    _intervalTimer.stop();

    updateCountdownWidgets();
}


void SummaryPage::timeout()
{
    logInfo() << "Summary page timeout -> finishing" << endl;
    // _ui->countdownLabel->setText( "0" );
    MainWindow::processEvents();

    emit finish();
}


void SummaryPage::setupCountdownMenuButton()
{
    if ( _countdownMenu )
        return;

    _countdownMenu = new QMenu();
    CHECK_NEW( _countdownMenu );

    QActionGroup * actionGroup = new QActionGroup( _countdownMenu );
    CHECK_NEW( actionGroup );

    addMenuAction( actionGroup,  10 );
    addMenuAction( actionGroup,  30 );
    addMenuAction( actionGroup,  60 );
    addMenuAction( actionGroup, 300, _( "Close after 300 seconds (5 minutes)" ) );
    addMenuAction( actionGroup,   0, _( "Don't automatically close" ) );

    connect( _countdownMenu, SIGNAL( triggered         ( QAction * ) ),
             this,           SLOT  ( configureCountdown( QAction * ) ) );

    _ui->countdownMenuButton->setMenu( _countdownMenu );
}


void SummaryPage::addMenuAction( QActionGroup *  actionGroup,
                                 int             seconds,
                                 const QString & overrideText )
{
    QString text = overrideText;

    if ( text.isEmpty() )
        text = _( "Close after %1 seconds" ).arg( seconds );

    QAction * action = new QAction( text, _countdownMenu );
    CHECK_NEW( action );

    action->setData( seconds );
    actionGroup->addAction( action );
    _countdownMenu->addAction( action );
}


void SummaryPage::configureCountdown( QAction * action )
{
    if ( action )
    {
        _countdownSec = action->data().toInt();
        logDebug() << "Setting new countdown: " << _countdownSec << endl;

        startCountdown();  // this also updates the widgets
    }
}


void SummaryPage::readSettings()
{
    QSettings settings;
    settings.beginGroup( "SummaryPage" );

    _countdownSec = settings.value( "countdownSec", 10 ).toInt();

    settings.endGroup();

    logDebug() << "countdownSec from settings: " << _countdownSec << endl;
}


void SummaryPage::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "SummaryPage" );

    settings.setValue( "countdownSec", _countdownSec );

    settings.endGroup();
}

