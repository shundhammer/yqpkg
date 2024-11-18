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

  File:	      YQProgressBar.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQProgressBar_h
#define YQProgressBar_h

#include <QFrame>
#include <yui/YProgressBar.h>


class QString;
class YQWidgetCaption;
class QProgressBar;


class YQProgressBar : public QFrame, public YProgressBar
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQProgressBar( YWidget * 		parent,
		   const std::string &	label,
		   int			maxValue = 100 );
    /**
     * Destructor.
     **/
    virtual ~YQProgressBar();

    /**
     * Set the label (the caption above the progress bar).
     *
     * Reimplemented from YProgressBar.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Set the current progress value ( <= maxValue() ).
     *
     * Reimplemented from YProgressBar.
     **/
    virtual void setValue( int newValue );

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
    QProgressBar *	_qt_progressbar;
};

#endif // YQProgressBar_h
