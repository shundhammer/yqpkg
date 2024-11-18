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

  File:	      YQLogView.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQLogView_h
#define YQLogView_h

#include <QFrame>
#include <QTextEdit>

#include <yui/YLogView.h>

class YQWidgetCaption;
class MyTextEdit;



class YQLogView : public QFrame, public YLogView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQLogView( YWidget * 	        parent,
	       const std::string & 	label,
	       int 		        visibleLines,
	       int 		        maxLines );

    /**
     * Destructor.
     **/
    virtual ~YQLogView();

protected:
    /**
     * Display the part of the log text that should be displayed.
     * 'text' contains the last 'visibleLines()' lines.
     * This is called whenever the log text changes. Note that the text might
     * also be empty, in which case the displayed log text should be cleared.
     *
     * Implemented from YLogView.
     **/
    virtual void displayLogText( const std::string & text );

public:

    /**
     * Set the label (the caption above the log text).
     *
     * Reimplemented from YLogView.
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


protected:

    YQWidgetCaption *	_caption;
    MyTextEdit *	_qt_text;
    QString             _lastText;

private slots:
    void slotResize();

};


// We need a resize event in order to set the cursor to the last line
// for the auto-scroll feature
class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyTextEdit( QWidget* parent ) : QTextEdit (parent) {}

protected:

    void resizeEvent ( QResizeEvent * event )
    {
        emit resized();
        QTextEdit::resizeEvent(event);
    }

signals:
    void resized();

};


#endif // YQLogView_h
