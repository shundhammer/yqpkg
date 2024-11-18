/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


/*
  File:	      YQPkgConflictDialog.cc
  Author:     Stefan Hundhammer <shundhammer.de>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include <yui/YUILog.h>

#include <yui/qt/YQUI.h>
#include <yui/qt/YQDialog.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>

#include <QLabel>
#include <QKeyEvent>
#include <QLayout>
#include <QMenu>
#include <QPushButton>
#include <QElapsedTimer>
#include <QPainter>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPixmap>
#include <QBoxLayout>

#include "YQPkgConflictDialog.h"
#include "YQPkgConflictList.h"
#include "QY2LayoutUtils.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget


// The busy dialog ("Checking Dependencies") will only be shown if solving
// (on average) takes longer than this many seconds. The first one will be
// shown in any case.

#define SUPPRESS_BUSY_DIALOG_SECONDS	1.5

using std::endl;

YQPkgConflictDialog::YQPkgConflictDialog( QWidget * parent )
    : QDialog( parent )
{
    setStyleSheet( QString() );

    _solveCount		= 0;
    _totalSolveTime	= 0.0;


    // Set the dialog title.
    //
    // "Dependency conflict" is already used as the conflict list header just
    // some pixels below that, so don't use this twice. This dialog title may
    // or may not be visible, depending on whether or not there is a window
    // manager running (and configured to show any dialog titles).

    setWindowTitle( _( "Warning" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );


    // Layout for the dialog (can't simply insert a QVbox)

    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);
    layout->setMargin(MARGIN);
    layout->setSpacing(SPACING);

    Q_CHECK_PTR( layout );

    // Conflict list

    _conflictList = new YQPkgConflictList( this );
    Q_CHECK_PTR( _conflictList );
    layout->addWidget( _conflictList );
    layout->addSpacing( 2 );

    connect( _conflictList, SIGNAL( updatePackages() ),
	     this,	    SIGNAL( updatePackages() ) );


    // Button box
    QHBoxLayout * buttonBox = new QHBoxLayout();
    Q_CHECK_PTR( buttonBox );
    buttonBox->setSpacing( SPACING );
    buttonBox->setMargin ( MARGIN  );
    layout->addLayout( buttonBox );
    buttonBox->addStretch();

    // "OK" button

    QPushButton * button = new QPushButton( _( "&OK -- Try Again" ), this);
    buttonBox->addWidget(button);
    Q_CHECK_PTR( button );
    button->setDefault( true );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( solveAndShowConflicts() ) );


    // "Expert" menu button

    button = new QPushButton( _( "&Expert" ), this );
    buttonBox->addWidget(button);

    Q_CHECK_PTR( button );


    // "Expert" menu

    _expertMenu = new QMenu( button );
    Q_CHECK_PTR( _expertMenu );
    button->setMenu( _expertMenu );

    _expertMenu->addAction( _( "&Save This List to a File..." ),
			     _conflictList, SLOT( askSaveToFile() ) );


    // "Cancel" button

    button = new QPushButton( _( "&Cancel" ), this);
    buttonBox->addWidget(button);
    Q_CHECK_PTR( button );

    connect( button, SIGNAL( clicked() ),
	     this,   SLOT  ( reject()  ) );
    buttonBox->addStretch();


    // Busy popup

    _busyPopup = new QLabel( "   " + _( "Checking Dependencies..." ) + "   ", parent );
    Q_CHECK_PTR( _busyPopup );

    _busyPopup->setWindowTitle( "" );
    _busyPopup->resize( _busyPopup->sizeHint() );
    YQDialog::center( _busyPopup, parent );


    // Here comes a real nasty hack.
    //
    // The busy popup is needed to indicate that the application is (you
    // guessed right) busy. But as long as it is busy, it doesn't process X
    // events, either, and I didn't manage to convince Qt to please paint this
    // popup before the solver's calculations (which take quite a while) start
    // - all combinations of show(), repaint(), XSync(), XFlush(),
    // processEvents() etc. failed.
    //
    // So, let's do it the hard way: Give this popup a background pixmap into
    // which we render the text to display. The X server draws background
    // pixmaps immediately, so we don't have to wait until the X server, the
    // window manager and this application are finished negotiating all their
    // various events.

    // Create a pixmap. Make it large enough so it isn't replicated (i.e. the
    // text is displayed several times) if some window manager chooses not to
    // honor the size hints (KDM for example uses double the height we
    // request).

    QSize size = _busyPopup->sizeHint();
    QPixmap pixmap( 3 * size.width(), 3 * size.height() );

    // Clear the pixmap with the widget's normal background color.
    //FIXME pixmap.fill( _busyPopup->paletteBackgroundColor() );

    // Render the text - aligned top and left because otherwise it will of
    // course be centered inside the pixmap which is usually much larger than
    // the popup, thus the text would be cut off.
    QPainter painter( &pixmap );
    painter.drawText( pixmap.rect(), Qt::AlignLeft | Qt::AlignTop, _busyPopup->text() );
    painter.end();

    //FIXME _busyPopup->setPaletteBackgroundPixmap( pixmap );

    // If the application manages to render the true contents of the label we
    // just misused so badly, the real label will interfere with the background
    // pixmap with (maybe) a few pixels offset (bug #25647). Fast or
    // multiprocessor machines tend to have this problem.
    // So let's get rid of the label text and solely rely on the background
    // pixmap.
    _busyPopup->setText( "" );

    // Make sure the newly emptied text doesn't cause the busy dialog to be
    // resized to nil (or a window manager dependent minimum size).
    _busyPopup->setFixedSize( _busyPopup->size() );
}


YQPkgConflictDialog::~YQPkgConflictDialog()
{
    // NOP
}


QSize
YQPkgConflictDialog::sizeHint() const
{
    return limitToScreenSize( this, 550, 450 );
}


int
YQPkgConflictDialog::solveAndShowConflicts()
{
    prepareSolving();

    yuiDebug() << "Solving..." << endl;
    QElapsedTimer solveTime;
    solveTime.start();

    // Solve.

    bool success = zypp::getZYpp()->resolver()->resolvePool();

    _totalSolveTime += solveTime.elapsed() / 1000.0;

    yuiDebug() << "Solving done in " << ( solveTime.elapsed() / 1000.0 )
	       << " s - average: "  << " s" << averageSolveTime()
	       << endl;

    return processSolverResult( success );
}


int
YQPkgConflictDialog::verifySystem()
{
    prepareSolving();

    yuiDebug() << "Verifying system..." << endl;
    QElapsedTimer solveTime;
    solveTime.start();

    bool success = zypp::getZYpp()->resolver()->verifySystem(); // considerNewHardware

    yuiDebug() << "System verified in " << solveTime.elapsed() / 1000.0 << " s" << endl;

    return processSolverResult( success );
}


void
YQPkgConflictDialog::prepareSolving()
{
    Q_CHECK_PTR( _conflictList );
    YQUI::ui()->busyCursor();

    if ( isVisible() )
    {
	// This is not only the starting point for all the dependency solving
	// magic, it is also used internally when clicking the "OK - Try again"
	// button. Thus, before doing anything else, check if the conflict list
	// still contains anything, and if so, apply any conflict resolutions
	// the user selected - but only if this dialog is already visible.

	_conflictList->applyResolutions();
    }


    // Initialize for next round of solving.
    _conflictList->clear();

    if ( _solveCount++ == 0 || averageSolveTime() > SUPPRESS_BUSY_DIALOG_SECONDS )
    {
	YQDialog::center( _busyPopup, parentWidget() );
	_busyPopup->show();

	// No _busyPopup->repaint() - that doesn't help anyway: Qt doesn't do
	// any actual painting until the window is mapped. We just rely on the
	// background pixmap we provided in the constructor.

        // Make sure show() gets processed - usually, a window manager catches
        // the show() (XMap) events, positions and maybe resizes the window and
        // only then sends off an event that makes the window appear. This
        // event needs to be processed.
	qApp->processEvents();
    }
}


int
YQPkgConflictDialog::processSolverResult( bool success )
{
    if ( _busyPopup->isVisible() )
	_busyPopup->hide();

    // Package states may have changed: The solver may have set packages to
    // autoInstall or autoUpdate. Make those changes known.
    emit updatePackages();

    YQUI::ui()->normalCursor();
    int result = QDialog::Accepted;

    if ( success )	// Solving went without any complaints?
    {
	result = QDialog::Accepted;

	if ( isVisible() )
	    accept();	// Pop down the dialog.
    }
    else		// There were solving problems.
    {
	yuiDebug() << "Dependency conflict!" << endl;
	YQUI::ui()->busyCursor();

	_conflictList->fill( zypp::getZYpp()->resolver()->problems() );
	YQUI::ui()->normalCursor();

	if ( ! isVisible() )
	{
	    // Pop up the dialog and run a local event loop.
	    result = exec();
	}
    }

    return result;	// QDialog::Accepted or QDialog::Rejected
}


void
YQPkgConflictDialog::resetIgnoredDependencyProblems()
{
    zypp::getZYpp()->resolver()->undo();
}


double
YQPkgConflictDialog::averageSolveTime() const
{
    if ( _solveCount < 1 )
	return 0.0;

    return _totalSolveTime / _solveCount;
}


void
YQPkgConflictDialog::askCreateSolverTestCase()
{
    QString testCaseDir = "/var/log/YaST2/solverTestcase";
    // Heading for popup dialog
    QString heading = QString( "<h2>%1</h2>" ).arg( _( "Create Dependency Resolver Test Case" ) );

    QString msg =
	_( "<p>Use this to generate extensive logs to help tracking down bugs in the dependency resolver. "
	   "The logs will be stored in directory <br><tt>%1</tt></p>" ).arg( testCaseDir );

    int button_no = QMessageBox::information( 0,			// parent
					      _( "Solver Test Case" ),	// caption
					      heading + msg,
					      _( "C&ontinue" ),		// button #0
					      _( "&Cancel" ) );		// button #1

    if ( button_no == 1 )	// Cancel
	return;

    yuiMilestone() << "Generating solver test case START" << endl;
    bool success = zypp::getZYpp()->resolver()->createSolverTestcase( qPrintable( testCaseDir ) );
    yuiMilestone() << "Generating solver test case END" << endl;

    if ( success )
    {
	msg =
	    _( "<p>Dependency resolver test case written to <br><tt>%1</tt></p>"
	       "<p>Prepare <tt>y2logs.tgz tar</tt> archive to attach to Bugzilla?</p>" ).arg( testCaseDir ),
	button_no = QMessageBox::question( 0,				// parent
					   _( "Success" ),		// caption
					   msg,
					   QMessageBox::Yes    | QMessageBox::Default,
					   QMessageBox::No,
					   QMessageBox::Cancel | QMessageBox::Escape );

	if ( button_no & QMessageBox::Yes ) // really binary (not logical) '&' - QMessageBox::Default is still in there
	    YQUI::ui()->askSaveLogs();
    }
    else // no success
    {
	QMessageBox::warning( 0,					// parent
			      _( "Error" ),				// caption
			      _( "<p><b>Error</b> creating dependency resolver test case</p>"
				 "<p>Please check disk space and permissions for <tt>%1</tt></p>" ).arg( testCaseDir ),
			      QMessageBox::Ok | QMessageBox::Default,
			      QMessageBox::NoButton,
			      QMessageBox::NoButton );
    }
}

void
YQPkgConflictDialog::keyPressEvent( QKeyEvent * event )
{
    if ( event &&  event->key() == Qt::Key_Print )
    {
        YQUI::ui()->makeScreenShot( "" );
        return;
    }
    QWidget::keyPressEvent( event );
}



