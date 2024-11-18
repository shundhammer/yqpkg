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

  File:	      YQMultiLineEdit.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQMultiLineEdit_h
#define YQMultiLineEdit_h

#include <QFrame>
#include <yui/YMultiLineEdit.h>

class YQWidgetCaption;
class QTextEdit;


/**
 * MultiLineEdit - an input area for multi-line text.
 **/
class YQMultiLineEdit : public QFrame, public YMultiLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQMultiLineEdit( YWidget * parent, const std::string & label );

    /**
     * Destructor.
     **/
    virtual ~YQMultiLineEdit();

    /**
     * Get the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Implemented from YMultiLineEdit.
     **/
    virtual std::string value();

    /**
     * Set the current value (the text entered by the user or set from the
     * outside) of this MultiLineEdit.
     *
     * Implemented from YMultiLineEdit.
     **/
    virtual void setValue( const std::string & text );

    /**
     * Set the label (the caption above the MultiLineEdit).
     *
     * Reimplemented from YMultiLineEdit.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Set the maximum input length, i.e., the maximum number of characters the
     * user can enter. -1 means no limit.
     *
     * Reimplemented from YMultiLineEdit.
     **/
    virtual void setInputMaxLength( int numberOfChars );

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


protected slots:

    /**
     * Triggered when the text changes.
     **/
    void changed();

    /**
     * Enforce the maximum input length: If the text becomes too long, remove
     * the just-entered character at the current cursor position.
     *
     * Note that this is a lot more user friendly than silently truncating at
     * the end of the text: In the latter case, chances are that the user never
     * gets to know that text was truncated. Removing the just-typed character
     * OTOH is something he will notice very quickly.
     **/
    void enforceMaxInputLength();


protected:

    YQWidgetCaption *	_caption;
    QTextEdit *		_qt_textEdit;
};


#endif // YQMultiLineEdit_h
