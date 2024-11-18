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

  File:	      YQRadioButton.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQRadioButton_h
#define YQRadioButton_h

#include <yui/YRadioButton.h>
#include <QRadioButton>

using std::string;


class YQRadioButton : public QRadioButton, public YRadioButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQRadioButton( YWidget *		parent,
		   const std::string &	label,
		   bool 		checked );

    /**
     * Return the "checked" state of the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual bool value();

    /**
     * Set the "checked" state of the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setValue( bool checked );

    /**
     * Change the label (the text) on the RadioButton.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Use a bold font.
     *
     * Reimplemented from YRadioButton.
     **/
    virtual void setUseBoldFont( bool bold = true );

    /**
     * Set enabled / disabled state.
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
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

protected slots:
    /**
     * Triggered when the RadioButton is toggled.
     **/
    void changed( bool newState );

    /**
     * Redirect events to this object.
     **/
    bool eventFilter( QObject * obj, QEvent * event );

};

#endif // YQRadioButton_h
