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

  File:	      YQMainWinDock.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QResizeEvent>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <yui/YEvent.h>

#include "YQDialog.h"
#include "YQMainWinDock.h"
#include "YQWizard.h"
#include "YQUI.h"

#define VERBOSE_RESIZE 1


YQMainWinDock *
YQMainWinDock::mainWinDock()
{
    static YQMainWinDock * mainWinDock = 0;

    if ( ! mainWinDock )
	mainWinDock = new YQMainWinDock();

    return mainWinDock;
}


YQMainWinDock::YQMainWinDock()
    : QWidget( 0, // parent, name
	       YQUI::ui()->noBorder() ?
	       Qt::FramelessWindowHint :
	       Qt::Window ),
      _sideBarWidth( 0 )
{
    setFocusPolicy( Qt::StrongFocus );

    resize( YQUI::ui()->defaultSize( YD_HORIZ ),
	    YQUI::ui()->defaultSize( YD_VERT  ) );

    yuiDebug() << "MainWinDock initial size: "
	       << size().width() << " x " << size().height()
	       << endl;
}


YQMainWinDock::~YQMainWinDock()
{
    // NOP
}


void
YQMainWinDock::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	resize( event->size() );
	resizeVisibleChild();
    }
}


void
YQMainWinDock::resizeVisibleChild()
{
    for ( YQWidgetStack::reverse_iterator it = _widgetStack.rbegin(); it != _widgetStack.rend(); it++ )
    {
	YQDialog * dialog = *it;

        QRect rect = QRect( QPoint( 0, 0 ), size() );

        YQWizard * wizard = dialog->findWizard();

        if ( wizard )
	    yuiDebug() << dialog << " with " << wizard << " isSecondary: " << std::boolalpha << wizard->isSecondary() << endl;

        if ( wizard && wizard->isSecondary() )
	{
	    if ( QApplication::isLeftToRight() )
		rect.setLeft( 0 );
	    else
		rect.setWidth( rect.width() );
	}

	if ( dialog->rect() != rect )
	{
#if VERBOSE_RESIZE
	    yuiDebug() << "Resizing child dialog " << std::hex << ( (void *) dialog ) << std::dec
		       << " to " << rect.width() << " x " << rect.height()
		       << endl;
#endif
            dialog->setGeometry( rect );
	}
    }
}


void
YQMainWinDock::show()
{
    QWidget::show();
    showCurrentDialog();
}


void
YQMainWinDock::add( YQDialog * dialog )
{
    YUI_CHECK_PTR( dialog );

    // Deactivate the next-lower dialog
    // (the one that currently still is the topmost on the _widgetStack)
    activateCurrentDialog( false );

    raiseDialog( dialog );

    yuiDebug() << "Adding dialog " << std::hex << (void *) dialog << std::dec
	       << "  to mainWinDock"
	       << endl;

    _widgetStack.push_back( dialog );
    resizeVisibleChild();

    show();
}


void
YQMainWinDock::raiseDialog( YQDialog * newCurrentDialog )
{
    if ( ! newCurrentDialog )
        return;

    newCurrentDialog->raise();
    newCurrentDialog->show();
    newCurrentDialog->update();

    for ( YQMainWinDock::YQWidgetStack::iterator it = _widgetStack.begin();
	  it != _widgetStack.end();
	  ++it )
    {
	if ( *it != newCurrentDialog )
        {
            QWidget * widget = (QWidget *) (*it)->widgetRep();
            widget->hide();
        }
    }
}


void
YQMainWinDock::showCurrentDialog()
{
    raiseDialog( topmostDialog() );
}


void
YQMainWinDock::activateCurrentDialog( bool active )
{
    if ( _widgetStack.empty() )
	return;

    // In the normal case, the (still or again) topmost dialog needs to be
    // activated or deactivated directly. Since this is done on the QWidget
    // level, its widgetRep() is needed -- which may or may not be the same as
    // the YQDialog.

    YQDialog * dialog = topmostDialog();

    if ( dialog )
    {
        QWidget * widget = (QWidget *) dialog->widgetRep();
	widget->setEnabled( active );
    }
}


void
YQMainWinDock::remove( YQDialog * dialog )
{
    if ( _widgetStack.empty() )
	return;

    if ( ! dialog )
	dialog = topmostDialog();

    if ( dialog == topmostDialog() )
    {
	// The most common case:
	// The topmost dialog is to be removed

	_widgetStack.pop_back();

	yuiDebug() << "Removing dialog " << std::hex << (void *) dialog << std::dec
		   <<" from mainWinDock"
		   << endl;
    }
    else // The less common (but more generic) case: Remove any dialog
    {
	YQMainWinDock::YQWidgetStack::iterator pos = findInStack( dialog );

	if ( pos == _widgetStack.end() )
	    return;

	yuiWarning() << "Found dialog somewhere in the middle of the widget stack" << endl;
	yuiDebug() << "Removing dialog " << std::hex << (void *) dialog << std::dec
		   << " from mainWinDock"
		   << endl;

	_widgetStack.erase( pos );
    }

    if ( _widgetStack.empty() )		// No more main dialog?
	hide();				// -> hide dock
    else
    {
	raiseDialog( topmostDialog() );	// Raise the next dialog from the stack
	activateCurrentDialog( true );
	resizeVisibleChild();
    }
}


YQMainWinDock::YQWidgetStack::iterator
YQMainWinDock::findInStack( YQDialog * dialog )
{
    for ( YQMainWinDock::YQWidgetStack::iterator it = _widgetStack.begin();
	  it != _widgetStack.end();
	  ++it )
    {
	if ( *it == dialog )
	    return it;
    }

    return _widgetStack.end();
}


YQDialog *
YQMainWinDock::topmostDialog() const
{
    if ( _widgetStack.empty() )
	return 0;
    else
	return _widgetStack.back();
}


bool
YQMainWinDock::couldDock()
{
    YDialog * topDialog = YDialog::topmostDialog( false ); // don't throw

    if ( ! topDialog )	// No dialog at all?
	return true;	// Can dock the next one without problems

    // The next dialog can be docked if there is no popup dialog currently open.
    // This is equivalent to the topmost dialog on the YDialog stack being the
    // same dialog as the topmost dialog of this MainWinDock.

    return topDialog->widgetRep() == this->topmostDialog();
}


void
YQMainWinDock::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button (and WM menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the Id( :cancel ) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    yuiMilestone() << "Caught window manager close event - returning with YCancelEvent" << endl;
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQMainWinDock::paintEvent( QPaintEvent * event )
{
    // NOP
}


void
YQMainWinDock::setSideBarWidth( int width )
{
    if ( _sideBarWidth == width )
        return;

    _sideBarWidth = width;
    resizeVisibleChild();
}
