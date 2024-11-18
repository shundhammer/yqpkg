/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2022 SUSE LLC
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

  File:	      	YQDialog.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

  Textdomain	"qt"

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <yui/YApplication.h>
#include <yui/YDialogSpy.h>
#include <yui/YEvent.h>

#include <QPushButton>
#include <QMessageBox>
#include <QDesktopWidget>

#include "QY2StyleEditor.h"
#include "QY2Styler.h"

#include "YQDialog.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YQGenericButton.h"
#include "YQMainWinDock.h"
#include "YQWizard.h"
#include "YQWizardButton.h"
#include "YQi18n.h"
#include "utf8.h"


#define YQMainDialogWFlags	Qt::Widget
#define YQPopupDialogWFlags     Qt::Dialog

#define VERBOSE_EVENT_LOOP	0

using std::endl;



YQDialog::YQDialog( YDialogType 	dialogType,
		    YDialogColorMode	colorMode )
: QWidget( chooseParent( dialogType ),
           dialogType == YPopupDialog ? YQPopupDialogWFlags : YQMainDialogWFlags )
, YDialog( dialogType, colorMode )
{
    setWidgetRep( this );

    _userResized   	= false;
    _focusButton   	= 0;
    _defaultButton 	= 0;
    _highlightedChild	= 0;
    _styleEditor	= 0;

    setFocusPolicy( Qt::StrongFocus );
    setAutoFillBackground( true );

    if ( colorMode != YDialogNormalColor )
    {
	QColor normalBackground     ( 240, 100, 36 );
	QColor inputFieldBackground ( 0xbb, 0xff, 0xbb );
	QColor text = Qt::black;

	if ( colorMode == YDialogInfoColor )
	{
	    normalBackground = QColor ( 238, 232, 170 ); // PaleGoldenrod
	}

	QPalette warnPalette( normalBackground );
	warnPalette.setColor( QPalette::Text, text );
	warnPalette.setColor( QPalette::Base, inputFieldBackground );
	setPalette( warnPalette );
    }

    qApp->setApplicationName(YQUI::ui()->applicationTitle());
    topLevelWidget()->setWindowTitle ( YQUI::ui()->applicationTitle() );
    QGuiApplication::setApplicationDisplayName( YQUI::ui()->applicationTitle() );

    if ( isMainDialog() && QWidget::parent() != YQMainWinDock::mainWinDock() )
    {
	setWindowFlags( YQPopupDialogWFlags );
    }

    if ( ! isMainDialog() )
        setWindowModality( Qt::ApplicationModal );

    if ( isMainDialog() && QWidget::parent() == YQMainWinDock::mainWinDock() )
    {
        YQMainWinDock::mainWinDock()->add( this );
    }

    _eventLoop  = new QEventLoop( this );
    YUI_CHECK_NEW( _eventLoop );

    _waitForEventTimer = new QTimer( this );
    YUI_CHECK_NEW( _waitForEventTimer );
    _waitForEventTimer->setSingleShot( true );

    QObject::connect( _waitForEventTimer, 	&pclass(_waitForEventTimer)::timeout,
		      this,			&pclass(this)::waitForEventTimeout );

    if ( isMainDialog() && QWidget::parent() == YQMainWinDock::mainWinDock() )
        QY2Styler::styler()->registerWidget( YQMainWinDock::mainWinDock() );
    else
        QY2Styler::styler()->registerWidget( this );
}


YQDialog::~YQDialog()
{
    if ( isMainDialog() )
    {
	YQMainWinDock::mainWinDock()->remove( this );
	// orphaned main dialogs are handled gracefully in YQWMainWinDock::remove()
    }

    if ( _defaultButton )
        _defaultButton->forgetDialog();

    if ( _focusButton )
        _focusButton->forgetDialog();

    if ( _styleEditor )
        delete _styleEditor;

    if ( isMainDialog() && QWidget::parent() == YQMainWinDock::mainWinDock() )
        QY2Styler::styler()->unregisterWidget( YQMainWinDock::mainWinDock() );
    else
        QY2Styler::styler()->unregisterWidget( this );
}


QWidget *
YQDialog::chooseParent( YDialogType dialogType )
{
    if ( dialogType == YPopupDialog)
        return popupParent();

    QWidget * parent = YQMainWinDock::mainWinDock()->window();

    if ( ( dialogType == YMainDialog || dialogType == YWizardDialog ) &&
	 YQMainWinDock::mainWinDock()->couldDock() )
    {
	yuiDebug() << "Adding dialog to mainWinDock" << endl;
	parent = YQMainWinDock::mainWinDock();
    }

    return parent;
}


void
YQDialog::openInternal()
{
    ensureOnlyOneDefaultButton();
    QWidget::show();
    QWidget::raise(); // FIXME: is this really necessary?
    QWidget::update();
}


void
YQDialog::activate()
{
    QWidget::raise();
    QWidget::update();
}


int
YQDialog::preferredWidth()
{
    int preferredWidth;

    if ( isMainDialog() )
    {
	if ( userResized() )
	    preferredWidth = _userSize.width();
	else
	    preferredWidth = YQUI::ui()->defaultSize( YD_HORIZ );
    }
    else
    {
	preferredWidth = YDialog::preferredWidth();
    }

    int screenWidth = qApp->desktop()->width();

    if ( preferredWidth > screenWidth )
    {
	yuiWarning() << "Limiting dialog width to screen width (" << screenWidth
		     << ") instead of " << preferredWidth
		     << " - check the layout!"
		     << endl;
    }

    return preferredWidth;
}


int
YQDialog::preferredHeight()
{
    int preferredHeight;

    if ( isMainDialog() )
    {
	if ( userResized() )
	    preferredHeight = _userSize.height();
	else
	    preferredHeight = YQUI::ui()->defaultSize( YD_VERT );
    }
    else
    {
	preferredHeight = YDialog::preferredHeight();
    }

    int screenHeight = qApp->desktop()->height();

    if ( preferredHeight > screenHeight )
    {
	yuiWarning() << "Limiting dialog height to screen height (" << screenHeight
		     << ") instead of " << preferredHeight
		     << " - check the layout!"
		     << endl;
    }

    return preferredHeight;
}


void
YQDialog::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YDialog::setEnabled( enabled );
}


void
YQDialog::setSize( int newWidth, int newHeight )
{
    // yuiDebug() << "Resizing dialog to " << newWidth << " x " << newHeight << endl;

    if ( newWidth > qApp->desktop()->width() )
	newWidth = qApp->desktop()->width();

    if ( newHeight > qApp->desktop()->height() )
	newHeight = qApp->desktop()->height();

    resize( newWidth, newHeight );

    if ( hasChildren() )
    {
        firstChild()->setSize( newWidth, newHeight );
        ( ( QWidget* )firstChild()->widgetRep() )->show();
    }
}


void
YQDialog::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	// yuiDebug() << "Resize event: " << event->size().width() << " x " << event->size().height() << endl;
	setSize ( event->size().width(), event->size().height() );
	_userSize = event->size();

	if ( QWidget::parent() )
	    _userResized = true;
    }
}


YQGenericButton *
YQDialog::findDefaultButton()
{
    if ( _defaultButton )
	return _defaultButton;

    _defaultButton = findDefaultButton( childrenBegin(), childrenEnd() );

    YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
    YDialog::setDefaultButton( _defaultButton );

    return _defaultButton;
}


YQGenericButton *
YQDialog::findDefaultButton( YWidgetListConstIterator begin,
			     YWidgetListConstIterator end ) const
{
    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YWidget * widget = *it;

	//
	// Check this widget
	//

	YQGenericButton * button = dynamic_cast<YQGenericButton *> (widget);

	if ( button && button->isDefaultButton() )
	{
	    return button;
	}


	//
	// Recurse over the children of this widget
	//

	if ( widget->hasChildren() )
	{
	    button = findDefaultButton( widget->childrenBegin(),
					widget->childrenEnd() );
	    if ( button )
		return button;
	}
    }

    return 0;
}


YQWizard *
YQDialog::ensureOnlyOneDefaultButton( YWidgetListConstIterator begin,
				      YWidgetListConstIterator end )
{
    YQGenericButton * def  = _focusButton ? _focusButton : _defaultButton;
    YQWizard * wizard = 0;

    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YQGenericButton * button       = dynamic_cast<YQGenericButton *> (*it);
	YQWizardButton  * wizardButton = dynamic_cast<YQWizardButton * > (*it);

	if ( ! wizard )
	    wizard = dynamic_cast<YQWizard *> (*it);

	if ( wizardButton )
	{
	    wizardButton->showAsDefault( false );
	}
	else if ( button )
	{
	    if ( button->isDefaultButton() )
	    {
		if ( _defaultButton && button != _defaultButton )
		{
		    yuiError() << "Too many default buttons: " << button  << endl;
		    yuiError() << "Using old default button: " << _defaultButton << endl;
		}
		else
		{
		    _defaultButton = button;
		}
	    }

	    if ( button->isShownAsDefault() && button != def )
		button->showAsDefault( false );
	}

	if ( (*it)->hasChildren() )
	{
	    YQWizard * wiz = ensureOnlyOneDefaultButton( (*it)->childrenBegin(),
							 (*it)->childrenEnd() );
	    if ( wiz )
		wizard = wiz;
	}
    }

    return wizard;
}


void
YQDialog::ensureOnlyOneDefaultButton()
{
    _defaultButton = 0;
    YQWizard * wizard = ensureOnlyOneDefaultButton( childrenBegin(), childrenEnd() );

    if ( ! _defaultButton && wizard )
    {
	_defaultButton = wizardDefaultButton( wizard );
    }

    if ( _defaultButton )
    {
	YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
	YDialog::setDefaultButton( _defaultButton );
    }


    YQGenericButton * def  = _focusButton ? _focusButton : _defaultButton;

    if ( def )
	def->showAsDefault();
}


YQWizard *
YQDialog::findWizard() const
{
    return findWizard( childrenBegin(), childrenEnd() );
}


YQWizard *
YQDialog::findWizard( YWidgetListConstIterator begin,
		      YWidgetListConstIterator end ) const
{
    for ( YWidgetListConstIterator it = begin; it != end; ++it )
    {
	YWidget * widget = *it;
	YQWizard * wizard = dynamic_cast<YQWizard *> (widget);

	if ( wizard )
	    return wizard;

	if ( widget->hasChildren() )
	{
	    wizard = findWizard( widget->childrenBegin(),
				 widget->childrenEnd() );
	    if ( wizard )
		return wizard;
	}
    }

    return 0;
}


YQGenericButton *
YQDialog::wizardDefaultButton( YQWizard * wizard ) const
{
    YQGenericButton * def = 0;

    if ( ! wizard )
	wizard = findWizard();

    if ( wizard )
    {
	// Pick one of the wizard buttons

	if ( wizard->direction() == YQWizard::Backward )
	{
	    if ( wizard->backButton()
		 && wizard->backButton()->isShown()
		 && wizard->backButton()->isEnabled() )
	    {
		def = wizard->backButton();
	    }
	}

	if ( ! def )
	{
	    if ( wizard->nextButton()
		 && wizard->nextButton()->isShown()
		 && wizard->nextButton()->isEnabled() )
	    {
		def = wizard->nextButton();
	    }
	}
    }

    return def;
}


void
YQDialog::setDefaultButton( YPushButton * newDefaultButton )
{
    if ( _defaultButton	  &&
	 newDefaultButton &&
	 newDefaultButton != _defaultButton )
    {
	if ( dynamic_cast<YQWizardButton *>( _defaultButton ) )
	{
	    // Let app defined default buttons override wizard buttons
	    _defaultButton->setDefaultButton( false );
	}
	else
	{
	    yuiError() << "Too many Opt(:default) PushButtons: " << newDefaultButton << endl;
	    newDefaultButton->setDefaultButton( false );
	    return;
	}
    }

    _defaultButton = dynamic_cast<YQGenericButton*>(newDefaultButton);

    if ( _defaultButton )
    {
	_defaultButton->setDefaultButton( true );
	yuiDebug() << "New default button: " << _defaultButton << endl;

	if ( _defaultButton && ! _focusButton )
	{
	    _defaultButton->showAsDefault( true );
	    _defaultButton->setKeyboardFocus();
	}
    }


    YDialog::setDefaultButton( 0 ); // prevent complaints about multiple default buttons
    YDialog::setDefaultButton( _defaultButton );
}


bool
YQDialog::activateDefaultButton( bool warn )
{
    // Try the focus button first, if there is any.

    if ( _focusButton 		   &&
	 _focusButton->isEnabled() &&
	 _focusButton->isShownAsDefault() )
    {
	yuiDebug() << "Activating focus button: " << _focusButton << endl;
	_focusButton->activate();
	return true;
    }


    // No focus button - try the default button, if there is any.

    _defaultButton = findDefaultButton();

    if ( _defaultButton 		&&
	 _defaultButton->isEnabled() 	&&
	 _defaultButton->isShownAsDefault() )
    {
	yuiDebug() << "Activating default button: " << _defaultButton << endl;
	_defaultButton->activate();
	return true;
    }
    else
    {
	if ( warn )
	{
	    yuiWarning() << "No default button in this dialog - ignoring [Return]" << endl;
	}
    }

    return false;
}


void
YQDialog::losingFocus( YQGenericButton * button )
{
    if ( button == _focusButton )
    {
	if ( _focusButton && _focusButton != _defaultButton )
	    _focusButton->showAsDefault( false );

	_focusButton = 0;
    }

    if ( ! _focusButton && _defaultButton )
	_defaultButton->showAsDefault( true );
}


void
YQDialog::gettingFocus( YQGenericButton * button )
{
    if ( _focusButton && _focusButton != button )
	_focusButton->showAsDefault( false );

    if ( _defaultButton && _defaultButton != button )
	_defaultButton->showAsDefault( false );

    _focusButton = button;

    if ( _focusButton )
	_focusButton->showAsDefault( true );
}


void
YQDialog::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	if ( event->key() == Qt::Key_Print )
	{
	    YQUI::ui()->makeScreenShot( "" );
	    return;
	}
	else if ( event->key()       == Qt::Key_F3 &&	// Shift-F3: select a UI theme (QSS style sheet) by menu
		  event->modifiers() == Qt::ShiftModifier )
        {
            YQUI::yqApp()->askForWidgetStyle();
            return;
        }
	else if ( event->key()       == Qt::Key_F4 &&	// Shift-F4: toggle colors for vision impaired users
		  event->modifiers() == Qt::ShiftModifier )
	{
            toggleAlternateStyleSheet();
	    return;
	}
        else if ( event->key()       == Qt::Key_F6 &&	// Shift-F6: ask for a widget ID and send it
		  event->modifiers() == Qt::ShiftModifier )
        {
            YQUI::ui()->askSendWidgetID();
        }
	else if ( event->key()       == Qt::Key_F7 &&	// Shift-F7: toggle debug logging
		  event->modifiers() == Qt::ShiftModifier )
	{
	    YQUI::ui()->askConfigureLogging();
	    return;
	}
	else if ( event->key()       == Qt::Key_F8 &&	// Shift-F8: save y2logs
		  event->modifiers() == Qt::ShiftModifier )
	{
	    YQUI::ui()->askSaveLogs();
	    return;
	}
	else if ( event->modifiers() == Qt::NoModifier )	// No Ctrl / Alt / Shift etc. pressed
	{
	    if ( event->key() == Qt::Key_Return ||
		 event->key() == Qt::Key_Enter    )
	    {
		(void) activateDefaultButton();
		return;
	    }
	}
	else if ( event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) )
	{
	    // Qt-UI special keys - all with Ctrl-Shift-Alt

	    yuiMilestone() << "Caught YaST2 magic key combination" << endl;

	    if ( event->key() == Qt::Key_M )
	    {
		YQUI::ui()->toggleRecordMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_P )
	    {
		YQUI::ui()->askPlayMacro();
		return;
	    }
	    else if ( event->key() == Qt::Key_C )
	    {
		YQUI::ui()->sendEvent( new YSpecialKeyEvent("config") );
		return;
	    }
	    else if ( event->key() == Qt::Key_D )
	    {
		YQUI::ui()->sendEvent( new YDebugEvent() );
		return;
	    }
	    else if ( event->key() == Qt::Key_T )
	    {
		yuiMilestone() << "*** Dumping widget tree ***" << endl;
		dumpWidgetTree();
		yuiMilestone() << "*** Widget tree end ***" << endl;
		return;
	    }
	    else if ( event->key() == Qt::Key_Y )
	    {
		yuiMilestone() << "Opening dialog spy" << endl;
		YDialogSpy::showDialogSpy();
		YQUI::ui()->normalCursor();
	    }
	    else if ( event->key() == Qt::Key_X )
	    {
                int result;
		yuiMilestone() << "Starting xterm" << endl;
		result = system( "/usr/bin/xterm &" );
                if (result < 0)
                    yuiError() << "/usr/bin/xterm not found" << endl;
		return;
	    }
	    else if ( event->key() == Qt::Key_S )
	    {
		yuiMilestone() << "Opening style editor" << endl;
                _styleEditor = new QY2StyleEditor(this);
                _styleEditor->show();
                _styleEditor->raise();
                _styleEditor->activateWindow();
		return;
	    }

	}
    }

    QWidget::keyPressEvent( event );
}


void
YQDialog::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button (and WM menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the Id( :cancel ) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    yuiMilestone() << "Caught window manager close event - returning with YCancelEvent" << endl;
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQDialog::focusInEvent( QFocusEvent * event )
{
    // The dialog itself doesn't need or want the keyboard focus, but obviously
    // (since Qt 2.3?) it needs QFocusPolicy::StrongFocus for the default
    // button mechanism to work. So let's accept the focus and give it to some
    // child widget.

    if ( event->reason() == Qt::TabFocusReason )
    {
	focusNextPrevChild( true );
    }
    else
    {
	if ( _defaultButton )
	    _defaultButton->setKeyboardFocus();
	else
	    focusNextPrevChild( true );
    }
}


YEvent *
YQDialog::waitForEventInternal( int timeout_millisec )
{
    YQUI::ui()->forceUnblockEvents();
    _eventLoop->wakeUp();

    YEvent * event  = 0;

    _waitForEventTimer->stop();

    if ( timeout_millisec > 0 )
	_waitForEventTimer->start( timeout_millisec ); // single shot

    if ( qApp->focusWidget() )
	qApp->focusWidget()->setFocus();

    YQUI::ui()->normalCursor();

    if ( ! _eventLoop->isRunning() )
    {
#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Executing event loop for " << this << endl;
#endif
	_eventLoop->exec();

#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Event loop finished for " << this << endl;
#endif
    }
    else
    {
#if VERBOSE_EVENT_LOOP
	yuiDebug() << "Event loop still running for " << this << endl;
#endif
    }

    _waitForEventTimer->stop();
    event = YQUI::ui()->consumePendingEvent();


    // Prepare a busy cursor if the UI cannot respond to user input within the
    // next 200 milliseconds (if the application doesn't call waitForEvent()
    // within this time again)

    YQUI::ui()->timeoutBusyCursor();

    return event;
}


YEvent *
YQDialog::pollEventInternal()
{
    _eventLoop->wakeUp();

    YEvent * event = 0;

    _waitForEventTimer->stop(); // just in case it's still running

    if ( ! YQUI::ui()->pendingEvent() )
    {
	// Very short (10 millisec) event loop
	_eventLoop->processEvents( QEventLoop::AllEvents, 10 );
    }

    if ( YQUI::ui()->pendingEvent() )
	event = YQUI::ui()->consumePendingEvent();

    return event;
}


void
YQDialog::waitForEventTimeout()
{
    if ( ! YQUI::ui()->pendingEvent() && isTopmostDialog() )
    {
	// Don't override a pending event with a timeout event
        // and don't deliver the timeout event if another dialog opened in the
        // meantime

	YQUI::ui()->sendEvent( new YTimeoutEvent() );
    }
}


void
YQDialog::center( QWidget * dialog, QWidget * parent )
{
    if ( ! dialog || ! parent )
        return;

    QPoint pos( ( parent->width()  - dialog->width()  ) / 2,
                ( parent->height() - dialog->height() ) / 2 );

    pos += parent->mapToGlobal( QPoint( 0, 0 ) );
    pos = dialog->mapToParent( dialog->mapFromGlobal( pos ) );
    dialog->move( pos );
}


void
YQDialog::highlight( YWidget * child )
{
    if ( _highlightedChild && _highlightedChild->isValid() )
    {
	// Un-highlight old highlighted child widget

	QWidget * qw = (QWidget *) _highlightedChild->widgetRep();

	if ( qw )
	{
	    qw->setPalette( _preHighlightPalette );
	    qw->setAutoFillBackground( _preHighlightAutoFill );
	}
    }

    _highlightedChild = child;

    if ( child )
    {
	QWidget * qw = (QWidget *) child->widgetRep();

	if ( qw )
	{
	    _preHighlightPalette  = qw->palette();
	    _preHighlightAutoFill = qw->autoFillBackground();

	    qw->setAutoFillBackground( true );
	    QPalette pal( QColor( 0xff, 0x66, 0x00 ) );	// Button color
	    pal.setBrush( QPalette::Window, QColor( 0xff, 0xaa, 0x00 ) ); // Window background
	    pal.setBrush( QPalette::Base  , QColor( 0xff, 0xee, 0x00 ) ); // Table etc. background

	    qw->setPalette( pal );
	}
    }
}


void
YQDialog::toggleAlternateStyleSheet()
{
    QY2Styler::styler()->toggleAlternateStyleSheet();

    if ( QY2Styler::styler()->usingAlternateStyleSheet() )
    {
        yuiMilestone() << "Switched to vision impaired palette" << endl;
        QMessageBox::information( this,                                         // parent
                                  _("Color switching"),  	                // caption
                                  _( "Switching to color palette for vision impaired users -\n"
                                     "press Shift-F4 again to switch back to normal colors."   ), // text
                                  QMessageBox::Ok | QMessageBox::Default,       // button0
                                  QMessageBox::NoButton,                        // button1
                                  QMessageBox::NoButton );                      // button2
    }
}


QWidget *
YQDialog::popupParent()
{
    QWidget * parent = 0;
    YDialog * currentDialog = YDialog::currentDialog( false );

    if ( currentDialog )
        parent = (QWidget *) currentDialog->widgetRep();

    return parent;
}
