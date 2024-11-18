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

  File:	      YQMainWinDock.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQMainWinDock_h
#define YQMainWinDock_h

#include <deque>
#include <qwidget.h>

class QWidgetStack;
class YQDialog;


/**
 * Container window for YQDialogs of type YMainWindowDialog:
 *
 * This widget "swallows" any main dialogs it gets so only the topmost of them
 * is visible at any given time. It acts as a window stack for main dialogs,
 * making the next lower dialog on the stack visible as when the (previously)
 * topmost main dialog is closed.
 *
 * This widget also handles its own visibility accordingly: It is visible if
 * and only if it has a main dialog to display. It makes itself invisible when
 * there is no more main dialog to display, and it makes itself visible again
 * when a new main dialog appears.
 *
 * This widget can swallow an arbitrary number of main dialogs as they are
 * opened as long as there is no popup dialog in between.
 **/
class YQMainWinDock : public QWidget
{
    friend class YQWizard;

    Q_OBJECT

public:
    /**
     * Static method to access the singleton for this class.
     *
     * This creates the (one and only) instance of this class in the first
     * call. Subsequent calls simply return this instance.
     **/
    static YQMainWinDock * mainWinDock();

    /**
     * Add a dialog (the widgetRep() of a YQDialog) to the MainWinDock (on top
     * of its widget stack. The widget stack does not assume ownership of the
     * widget.
     *
     * If the MainWinDock is not visible yet, this operation makes it visible.
     **/
    void add( YQDialog * dialog );

    /**
     * Remove a dialog from the MainWinDock (if it belongs to the
     * MainWinDock). If dialog is 0, this removes the topmost dialog from the
     * MainWinDock.
     *
     * This can safely be called in the destructor of all dialogs, even those
     * that were never added to the MainWinDock.
     *
     * If that was the last main dialog in the MainWinDock, the MainWinDock
     * will be hidden (until another main dialog is added).
     **/
    void remove( YQDialog * dialog = 0 );

    /**
     * Return the current topmost dialog (the widgetRep() of a YQDialog)
     * or 0 if there is none.
     **/
    YQDialog * topmostDialog() const;

    /**
     * Return 'true' if the next main dialog could be docked,
     * i.e., if there is either no open dialog at all or only main dialogs.
     **/
    bool couldDock();

    /**
     * Show the widget (make it visible).
     *
     * Reimplemented from QWidget.
     **/
    virtual void show();

    /**
     * Window manager close event (Alt-F4):
     * Send a YCancelEvent and let the application handle that event.
     *
     * Reimplemented from QWidget.
     **/
    virtual void closeEvent( QCloseEvent * event );


protected:
    /**
     * Constructor.
     *
     * Use the static mainWinDock() method to access the singleton for this
     * class.
     **/
    YQMainWinDock();

    /**
     * Destructor.
     **/
    virtual ~YQMainWinDock();

    /**
     * Paint event.
     *
     * Reimplemented from QWidget.
     **/
    virtual void paintEvent( QPaintEvent * event );

    /**
     * Resize event.
     *
     * Reimplemented from QWidget.
     **/
    virtual void resizeEvent( QResizeEvent * event );

    /**
     * Resize the visible child to the current size of the dock.
     **/
    void resizeVisibleChild();

    /**
     * For secondary wizards
     **/
    void setSideBarWidth( int width );

    /**
     * Activate or deactivate the next-lower dialog in the dock when a new
     * dialog is opened or when a dialog is closed.
     **/
    void activateCurrentDialog( bool active );


protected slots:

    /**
     * Show the current dialog.
     **/
    void showCurrentDialog();


private:

    typedef std::deque<YQDialog *> YQWidgetStack;

    /**
     * Return an iterator to the specified dialog in the internal
     * widgetstack or _widgetStack::end() if not found.
     **/
    YQWidgetStack::iterator findInStack( YQDialog * dialog );

    /**
     * Raise and show the specified dialog and hide all others.
     **/
    void raiseDialog( YQDialog * dialog );


    YQWidgetStack       _widgetStack;
    int                 _sideBarWidth;
};


#endif // YQMainWinDock_h
