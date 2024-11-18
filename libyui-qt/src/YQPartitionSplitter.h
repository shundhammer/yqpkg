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

  File:	      YQPartitionSplitter.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQPartitionSplitter_h
#define YQPartitionSplitter_h

#include "qwidget.h"
#include <yui/YPartitionSplitter.h>

class YLayoutBox;
class YQBarGraph;
class YQIntField;
class YQSlider;


class YQPartitionSplitter : public QWidget, public YPartitionSplitter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQPartitionSplitter( YWidget *              parent,
			 int                    usedSize,
			 int                    freeSize,
			 int                    newPartSize,
			 int                    minNewPartSize,
			 int                    minFreeSize,
			 const std::string &    usedLabel,
			 const std::string &	freeLabel,
			 const std::string &	newPartLabel,
			 const std::string &	freeFieldLabel,
			 const std::string &	newPartFieldLabel );

    /**
     * Destructor.
     **/
    ~YQPartitionSplitter();

    /**
     * Return the value (the size of the new partition).
     *
     * Implemented from YPartitionSplitter.
     **/
    virtual int value();

    /**
     * Set the value (the size of the new partition).
     *
     * Implemented from YPartitionSplitter.
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


public slots:
    /**
     * Slot for setting the free size.
     **/
    void setFreeSizeSlot( int newFreeSize );

    /**
     * Slot for setting the new size.
     **/
    void setNewPartSizeSlot(  int newNewSize );


protected:

    enum
    {
	usedSegment    = 0,
	freeSegment    = 1,
	newPartSegment = 2
    };


    //
    // Widgets
    //

    YLayoutBox *	_vbox;
    YQBarGraph *	    _barGraph;
    YLayoutBox *	    _hbox;
    YQSlider   *		_freeSizeSlider;
    YQIntField  *		_newPartField;

};


#endif // YQPartitionSplitter_h
