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

  File:	      YQDialog.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQDialog_h
#define YQDialog_h

#include <QFrame>
#include <QEvent>
#include <QWidget>
#include <QPalette>
#include <QEvent>
#include <QCloseEvent>

#include <yui/YDialog.h>

class YQGenericButton;
class YQWizard;
class QEventLoop;
class QTimer;
class QY2StyleEditor;


class YQDialog : public QWidget, public YDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * 'dialogType' is one of YMainDialog, YWizardDialog, or YPopupDialog.
     *
     * The Qt UI supports YWizardDialogs. They are handled very much like
     * YMainDialogs, except for wizard dialogs that are opened over a wizard
     * with a steps panel on the left side, in which case that new wizard
     * dialog will be resized and moved so the steps panel from the wizard
     * below will remain visible.
     *
     * 'colorMode' can be set to YDialogWarnColor to use very bright "warning"
     * colors or YDialogInfoColor to use more prominent, yet not quite as
     * bright as "warning" colors. Use both only very rarely.
     **/
    YQDialog( YDialogType	dialogType,
	      YDialogColorMode	colorMode = YDialogNormalColor );

protected:

    /**
     * Destructor.
     * Don't delete a dialog directly, use YDialog::deleteTopmostDialog().
     **/
    virtual ~YQDialog();


public:

    /**
     * Interited from QDialog: The window was closed via the window
     * manager close button.
     **/
    void closeEvent( QCloseEvent * ev );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Return this dialog's (first) default button or 0 if none
     **/
    YQGenericButton * findDefaultButton();

    /**
     * Return 'true' if the user resized this dialog.
     **/
    bool userResized() { return _userResized; }

    /**
     * Returns the button that has the keyboard focus or 0 if no button has
     * the keyboard focus.
     **/
    YQGenericButton * focusButton() const { return _focusButton; }

    /**
     * Returns the dialog's default button - the button that is activated with
     * [Return] if no button has the keyboard focus.
     **/
    YQGenericButton * defaultButton() const { return _defaultButton; }

    /**
     * Notification that a button loses the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they lose focus so
     * the dialog can keep track of its focusButton.
     **/
    void losingFocus( YQGenericButton * button );

    /**
     * Notification that a button gets the keyboard focus.
     *
     * All pushbuttons are required to call this whenever they gain focus so
     * the dialog can keep track of its focusButton.
     **/
    void gettingFocus( YQGenericButton * button );

    /**
     * Set the dialog's default button - the button that is activated with
     * [Return] if no other button has the keyboard focus.
     * 'newDefaultButton' may be 0 if the former default button is destroyed.
     **/
    void setDefaultButton( YPushButton * newDefaultButton );

    /**
     * Ensure presence of no more than one single default button.
     **/
    void ensureOnlyOneDefaultButton();

    /**
     * Activate (i.e. click) this dialog's default button, if there is any.
     * Issue a warning to the log file if 'warn' is true.
     **/
    bool activateDefaultButton( bool warn = true );

    /**
     * Find the first wizard in that dialog, if there is any.
     * Returns 0 if there is none.
     **/
    YQWizard * findWizard() const;

    /**
     * Find a wizard button that would make sense as a default button.
     * Return 0 if none can be found.
     **/
    YQGenericButton * wizardDefaultButton( YQWizard * wizard ) const;

    /**
     * Highlight a child widget of this dialog. This is meant for debugging:
     * YDialogSpy and similar uses.
     *
     * No more than one widget can be highlighted at any one time in the same
     * dialog. Highlighting another widget un-highlights a previously
     * highlighted widget. 0 means 'unhighlight the last highlighted widget,
     * but don't highlight any other'.
     *
     * Implemented from YDialog.
     **/
    virtual void highlight( YWidget * child );

    /**
     * Center a dialog relative to 'parent'.
     *
     * If 'parent' is 0, the dialog is centered relative to the application's
     * main widget. If 'dialog' is the main widget and 'parent' is 0, the
     * dialog is centered relative to the desktop.
     **/
    static void center( QWidget * dialog, QWidget * parent = 0 );

    /**
     * Access to this dialog's event loop.
     **/
    QEventLoop * eventLoop() { return _eventLoop; }


    /**
     * Return a suitable parent QWidget for a Qt pop-up dialog to maintain the
     * correct window stacking order (so the pop-up does not appear behind an
     * existing window). If there is none, this returns 0 which is also a
     * suitable parent.
     **/
    static QWidget * popupParent();


protected slots:

    /**
     * Timeout during waitForEvent()
     **/
    void waitForEventTimeout();


protected:

    /**
     * Switch between the normal widget theme (QSS style sheet) and the vision
     * impaired theme with very bright colors.
     **/
    void toggleAlternateStyleSheet();

    /**
     * Choose a parent widget for a dialog of the specified type:
     * Either the main window dock (if this is a YMainDialog and the dock
     * currently accepts child dialogs) or 0.
     **/
    static QWidget * chooseParent( YDialogType dialogType );

    /**
     * Return the (first) default button between 'begin' and 'end'
     * or 0 if there is none.
     **/
    YQGenericButton * findDefaultButton( YWidgetListConstIterator begin,
					 YWidgetListConstIterator end ) const;

    /**
     * Return the (first) wizard widget between 'begin' and 'end'
     * or 0 if there is none.
     **/
    YQWizard * findWizard( YWidgetListConstIterator begin,
			   YWidgetListConstIterator end ) const;

    /**
     * Helper function for ensureOnlyOneDefaultButton():
     * Recursively find all normal and wizard buttons between 'begin' and 'end'
     * and make sure that no more than one button is marked as default.
     * Return (the first) wizard widget found on the way.
     **/
    YQWizard * ensureOnlyOneDefaultButton( YWidgetListConstIterator begin,
					   YWidgetListConstIterator end );

    /**
     * Internal open() method, called exactly once during the life
     * time of the dialog in open().
     *
     * Implemented from YDialog.
     **/
    virtual void openInternal();

    /**
     * Wait for a user event.
     *
     * Implemented from YDialog.
     **/
    virtual YEvent * waitForEventInternal( int timeout_millisec );

    /**
     * Check if a user event is pending. If there is one, return it.
     * If there is none, do not wait for one - return 0.
     *
     * Implemented from YDialog.
     **/
    virtual YEvent * pollEventInternal();

    /**
     * Activate this dialog: Make sure that it is shown as the topmost dialog
     * of this application and that it can receive input.
     *
     * Implemented from YDialog.
     **/
    virtual void activate();

    /**
     * Qt event handlers.
     *
     * All reimplemented from QWidget.
     **/
    virtual void keyPressEvent	( QKeyEvent	* event );
    virtual void focusInEvent	( QFocusEvent	* event );
    virtual void resizeEvent	( QResizeEvent	* event );


    //
    // Data members
    //

    bool		_userResized;
    QSize		_userSize;

    YQGenericButton *	_focusButton;
    YQGenericButton *	_defaultButton;

    QTimer *		_waitForEventTimer;
    QEventLoop *	_eventLoop;
    YWidget *		_highlightedChild;
    QPalette		_preHighlightPalette;
    bool		_preHighlightAutoFill;
    QY2StyleEditor*     _styleEditor;

};


#endif // YQDialog_h

