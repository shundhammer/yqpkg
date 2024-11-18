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

  File:	      YQIntField.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQIntField_h
#define YQIntField_h

#include <qlabel.h>
#include <QFrame>

#include <yui/YIntField.h>


class YQWidgetCaption;
class QSpinBox;


class YQIntField : public QFrame, public YIntField
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQIntField( YWidget *	        parent,
		const std::string &	label,
		int 		        minValue,
		int 		        maxValue,
		int 		        initialValue );

    /**
     * Destructor.
     **/
    virtual ~YQIntField();

    /**
     * Get the current value (the number entered by the user or set from the
     * outside) of this IntField.
     *
     * Implemented from YIntField.
     **/
    virtual int value();

protected:

    /**
     * Set the current value (the number entered by the user or set from the
     * outside) of this IntField. 'val' is guaranteed to be between minValue
     * and maxValue; no further checks are required.
     *
     * Implemented from YIntField.
     **/
    virtual void setValueInternal( int val );

public:

    /**
     * Set the label (the caption above the input field).
     *
     * Reimplemented from YIntField.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Sets the widget's enabled state.
     *
     * Inherited from YWidget.
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


signals:

    /**
     * Emitted when the value changes (regardless of the notify flag).
     **/
    void valueChanged( int newValue );


protected slots:

    /**
     * Slot for "value changed". This will send a ValueChanged event if
     * 'notify' is set.
     **/
    void valueChangedSlot( int newValue );


protected:

    YQWidgetCaption *	_caption;
    QSpinBox *		_qt_spinBox;
};


#endif // YQIntField_h
