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

  File:	      YQPartitionSplitter.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <yui/YEvent.h>
#include <yui/YWidgetFactory.h>
#include <yui/YOptionalWidgetFactory.h>
#include <yui/YLayoutBox.h>
#include "utf8.h"

#include "YQUI.h"
#include "YQPartitionSplitter.h"
#include "YQBarGraph.h"
#include "YQIntField.h"
#include "YQSlider.h"
#include "YQSignalBlocker.h"

using std::string;


YQPartitionSplitter::YQPartitionSplitter( YWidget *		parent,
					  int			usedSize,
					  int			totalFreeSize,
					  int			newPartSize,
					  int			minNewSize,
					  int			minFreeSize,
					  const string &	usedLabel,
					  const string &	freeLabel,
					  const string &	newPartLabel,
					  const string &	freeFieldLabel,
					  const string &	newPartFieldLabel )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YPartitionSplitter( parent,
			  usedSize,
			  totalFreeSize,
			  newPartSize,
			  minNewSize,
			  minFreeSize,
			  usedLabel,
			  freeLabel,
			  newPartLabel,
			  freeFieldLabel,
			  newPartFieldLabel )
    , _vbox( 0 )
    , _barGraph( 0 )
    , _hbox( 0 )
    , _freeSizeSlider( 0 )
    , _newPartField( 0 )
{
    setWidgetRep( this );

    // Replace children manager so it will accept one single direct child (the outer vbox)
    setChildrenManager( new YSingleWidgetChildManager( this ) );

    //
    // Create internal widgets
    //

    _vbox 	= YUI::widgetFactory()->createVBox( this );
    _barGraph	= dynamic_cast<YQBarGraph *> ( YUI::optionalWidgetFactory()->createBarGraph( _vbox ) );
    YUI_CHECK_PTR( _barGraph );

    int freeSize = totalFreeSize - newPartSize;

    {
	YBarGraphMultiUpdate multiUpdate( _barGraph );

	_barGraph->addSegment( YBarGraphSegment( usedSize,    usedLabel ) );
	_barGraph->addSegment( YBarGraphSegment( freeSize,    freeLabel ) );
	_barGraph->addSegment( YBarGraphSegment( newPartSize, newPartLabel) );
    }

    _hbox = YUI::widgetFactory()->createHBox( _vbox );

    _freeSizeSlider = new YQSlider( _hbox, freeFieldLabel,
				    minFreeSize, maxFreeSize(), freeSize,
				    true ); // reverseLayout (put QSpinBox left of QSlider)
    YUI_CHECK_PTR( _freeSizeSlider );
    _freeSizeSlider->setStretchable( YD_HORIZ, true );

    _newPartField = new YQIntField( _hbox, newPartFieldLabel,
				    minNewSize, maxNewPartSize(), newPartSize );
    YUI_CHECK_PTR( _newPartField );
    _newPartField->setStretchable( YD_HORIZ, false );


    // Connect signals

    connect( _newPartField,	&pclass(_newPartField)::valueChanged,
	     this,		&pclass(this)::setNewPartSizeSlot );

    connect( _freeSizeSlider,	&pclass(_freeSizeSlider)::valueChanged,
	     this,		&pclass(this)::setFreeSizeSlot );
}


YQPartitionSplitter::~YQPartitionSplitter()
{
    // NOP
}


void YQPartitionSplitter::setEnabled( bool enabled )
{
    _freeSizeSlider->setEnabled( enabled );
    _newPartField->setEnabled  ( enabled );

    YWidget::setEnabled( enabled );
}


int YQPartitionSplitter::preferredWidth()
{
    return _vbox->preferredWidth();
}


int YQPartitionSplitter::preferredHeight()
{
    return _vbox->preferredHeight();
}


void YQPartitionSplitter::setSize( int newWidth, int newHeight )
{
    QWidget::resize( newWidth, newHeight );
    _vbox->setSize ( newWidth, newHeight );
}


int YQPartitionSplitter::value()
{
    YUI_CHECK_PTR( _newPartField );

    return _newPartField->value();
}


void YQPartitionSplitter::setValue( int newValue )
{
    YUI_CHECK_PTR( _barGraph       );
    YUI_CHECK_PTR( _freeSizeSlider );
    YUI_CHECK_PTR( _newPartField   );

    YQSignalBlocker sigBlocker1( _barGraph	 );
    YQSignalBlocker sigBlocker2( _freeSizeSlider );
    YQSignalBlocker sigBlocker3( _newPartField	 );

    _newPartField->setValue( newValue );

    int freeSize = totalFreeSize() - newValue;
    _freeSizeSlider->setValue( freeSize );

    YBarGraphMultiUpdate multiUpdate( _barGraph );
    {
	_barGraph->setValue( freeSegment,    freeSize );
	_barGraph->setValue( newPartSegment, newValue );
    }
}


void YQPartitionSplitter::setFreeSizeSlot( int newFreeSize )
{
    int newPartSize = totalFreeSize() - newFreeSize;

    setValue( newPartSize );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQPartitionSplitter::setNewPartSizeSlot( int newPartSize )
{
    setValue( newPartSize );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQPartitionSplitter::setKeyboardFocus()
{
    _newPartField->setKeyboardFocus();

    return true;
}



