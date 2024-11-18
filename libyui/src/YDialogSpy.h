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

  File:		YDialogSpy.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YDialogSpy_h
#define YDialogSpy_h

#include "ImplPtr.h"

class YWidget;
class YDialog;
class YDialogSpyPrivate;


/**
 * An interactive dialog debugger: Show the structure and content of a dialog
 * and its widgets.
 *
 * This can be invoked by special key combinations:
 * Ctrl-Alt-Shift-Y in the Qt UI
 **/
class YDialogSpy
{
public:
    /**
     * Show a YDialogSpy for the specified dialog. 0 means "use the topmost
     * dialog".
     * This will return only when the user closes the YDialogSpy dialog.
     **/
    static void showDialogSpy( YDialog * dialog = 0 );

    /**
     * Show the "Properties" sub-window.
     **/
    void showProperties();

    /**
     * Hide the "Properties" sub-window.
     **/
    void hideProperties();

    /**
     * Return 'true' if the "Properties" sub-window is currently shown,
     * 'false' if not.
     **/
    bool propertiesShown() const;

protected:
    /**
     * Constructor: Create a YDialogSpy for the specified dialog. 0 means "use
     * the topmost dialog".
     *
     * In most cases it is more useful to use the static showDialogSpy() method
     * rather than create this dialog directly.
     **/
    YDialogSpy( YDialog * dialog = 0 );

    /**
     * Destructor.
     **/
    virtual ~YDialogSpy();

    /**
     * Execute the event loop. This will only return when the user closes the
     * YDialogSpy dialog.
     **/
    void exec();

    /**
     * Show the properties of the specified widget if the "Properties"
     * sub-window is currently shown.
     **/
    void showProperties( YWidget * widget );

private:

    ImplPtr<YDialogSpyPrivate> priv;
};


#endif // YDialogSpy_h
