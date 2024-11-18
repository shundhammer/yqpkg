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

  File:	      YQRadioButtonGroup.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQRadioButtonGroup_h
#define YQRadioButtonGroup_h

#include <QWidget>
#include <yui/YRadioButtonGroup.h>


class YQRadioButtonGroup : public QWidget, public YRadioButtonGroup
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQRadioButtonGroup( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQRadioButtonGroup();

    /**
     * Add a RadioButton.
     *
     * Reimplemented from YRadioButtonGroup.
     **/
    virtual void addRadioButton( YRadioButton * button );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQRadioButtonGroup_h
