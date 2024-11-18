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

  File:	      YQCheckBoxFrame.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQCheckBoxFrame_h
#define YQCheckBoxFrame_h

#include <qgroupbox.h>
#include <yui/YCheckBoxFrame.h>


class QWidget;
class QCheckBox;

class YQCheckBoxFrame : public QGroupBox, public YCheckBoxFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQCheckBoxFrame( YWidget * 		parent,
		     const std::string &	label,
		     bool 		checked );

    /**
     * Change the label text on the CheckBoxFrame.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Check or uncheck the CheckBoxFrame's check box.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual void setValue( bool isChecked );

    /**
     * Get the status of the CheckBoxFrame's check box.
     *
     * Reimplemented from YCheckBoxFrame.
     **/
    virtual bool value();

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


private slots:

    /**
     * Triggered when the on/off status is changed
     **/
    void stateChanged( bool newState );


protected:

    /**
     * Reimplemented from QGroupBox to prevent QGroupBox from disabling
     * children according to the check box status as the children are inserted.
     **/
    virtual void childEvent( QChildEvent * );

    virtual bool event(QEvent *e);
};


#endif // YQCheckBoxFrame_h
