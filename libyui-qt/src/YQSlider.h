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

  File:	      YQSlider.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQSlider_h
#define YQSlider_h

#include <QFrame>
#include <yui/YSlider.h>

class YQWidgetCaption;
class QSlider;
class QSpinBox;

class YQSlider : public QFrame, public YSlider
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * 'reverseLayout' creates the QSpinBox to the left and the QSlider to the right.
     **/
    YQSlider( YWidget *		        parent,
	      const std::string &	label,
	      int 		        minValue,
	      int 		        maxValue,
	      int 		        initialValue,
	      bool		        reverseLayout = false );

    /**
     * Destructor.
     **/
    virtual ~YQSlider();

    /**
     * Get the current value (the number entered by the user or set from the
     * outside) of this slider.
     *
     * Implemented from YSlider / YIntField.
     **/
    virtual int value();

protected:

    /**
     * Set the current value (the number entered by the user or set from the
     * outside) of this slider. 'val' is guaranteed to be between minValue
     * and maxValue; no further checks are required.
     *
     * Implemented from YSlider/ YIntField.
     **/
    virtual void setValueInternal( int val );

public:

    /**
     * Set the label (the caption above the input field).
     *
     * Reimplemented from YSlider / YIntField.
     **/
    virtual void setLabel( const std::string & label );

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
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


public slots:
    /**
     * Slot for "value changed". This will send a ValueChanged event if
     * 'notify' is set.
     **/
    void valueChangedSlot( int newValue );

signals:

    /**
     * Emitted when the value changes (regardless of the notify flag).
     **/
    void valueChanged( int newValue );


protected:

    YQWidgetCaption *	_caption;
    QSlider *		_qt_slider;
    QSpinBox *		_qt_spinBox;
    QFrame *		_hbox;
};


#endif // YQSlider_h
