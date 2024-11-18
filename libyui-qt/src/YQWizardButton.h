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

  File:	      YQWizardButton.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQWizardButton_h
#define YQWizardButton_h

#include <qwidget.h>
#include "YQGenericButton.h"

using std::string;
class YQDialog;
class YQWizard;


class YQWizardButton : public YQGenericButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQWizardButton( YQWizard *		wizard,
		    QWidget *		buttonParent,
		    const std::string &	label );

    /**
     * Destructor.
     **/
    virtual ~YQWizardButton();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char *widgetClass() const { return "YQWizardButton"; }

    /**
     * Hide the associated QPushButton.
     **/
    void hide();

    /**
     * Show the associated QPushButton - not this widget itself (!).
     * This widget itself will never become visible.
     **/
    void show();

    /**
     * Returns 'true' if the associated QPushButton (!) is shown.
     **/
    bool isShown() const;

    /**
     * Returns 'true' if the associated QPushButton (!) is hidden.
     **/
    bool isHidden() const;

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
     * Returns the wizard this wizard button belongs to.
     **/
    YQWizard * wizard() const { return _wizard; }

signals:

    /**
     * Emitted when the button is clicked or otherwise activated.
     **/
    void clicked();


private:

    YQWizard * _wizard;
};

#endif // YQWizardButton_h
