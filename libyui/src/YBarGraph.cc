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

  File:		YBarGraph.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <stdio.h>
#include <vector>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YBarGraph.h"

using std::string;


#define CHECK_INDEX(index)						\
    do									\
    {									\
	if ( (index) < 0  ||						\
	     (index) >= (int) priv->segments.size() )			\
	{								\
	    YUI_THROW( YUIIndexOutOfRangeException(			\
                           (index), /* current */			\
			   0,	    /* min */				\
		           (int) priv->segments.size() - 1  ) ); /* max */ \
	}								\
    } while ( 0 )



struct YBarGraphPrivate
{
    YBarGraphPrivate()
	: updatesPending( false )
	, postponeUpdates( false )
	{}

    std::vector<YBarGraphSegment>	segments;
    bool				updatesPending;
    bool				postponeUpdates;
};




YBarGraph::YBarGraph( YWidget * parent )
    : YWidget( parent )
    , priv( new YBarGraphPrivate() )
{
    YUI_CHECK_NEW( priv );
    setDefaultStretchable( YD_HORIZ, true );
}


YBarGraph::~YBarGraph()
{
    // NOP
}


void
YBarGraph::updateDisplay()
{
    priv->updatesPending = true;

    if ( ! priv->postponeUpdates )
    {
	doUpdate();
	priv->updatesPending = false;
    }
}


void
YBarGraph::addSegment( const YBarGraphSegment & segment )
{
    priv->segments.push_back( segment );
    updateDisplay();
}


void
YBarGraph::deleteAllSegments()
{
    priv->segments.clear();
    updateDisplay();
}


const YBarGraphSegment &
YBarGraph::segment( int segmentIndex ) const
{
    CHECK_INDEX( segmentIndex );

    return priv->segments[ segmentIndex ];
}


int
YBarGraph::segments() const
{
    return (int) priv->segments.size();
}


void
YBarGraph::setValue( int segmentIndex, int newValue )
{
    CHECK_INDEX( segmentIndex );

    priv->segments[ segmentIndex ].setValue( newValue );
    updateDisplay();
}


void
YBarGraph::setLabel( int segmentIndex, const string & newLabel )
{
    CHECK_INDEX( segmentIndex );

    priv->segments[ segmentIndex ].setLabel( newLabel );
    updateDisplay();
}


void
YBarGraph::setSegmentColor( int segmentIndex, const YColor & color )
{
    CHECK_INDEX( segmentIndex );

    if ( color.isUndefined() )
	YUI_THROW( YUIException( "Invalid YColor" ) );

    priv->segments[ segmentIndex ].setSegmentColor( color );
    updateDisplay();
}


void
YBarGraph::setTextColor( int segmentIndex, const YColor & color )
{
    CHECK_INDEX( segmentIndex );

    if ( color.isUndefined() )
	YUI_THROW( YUIException( "Invalid YColor" ) );

    priv->segments[ segmentIndex ].setTextColor( color );
    updateDisplay();
}


const YPropertySet &
YBarGraph::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property list<integer>      Values	The numerical value for each segment.
	 * @property list<string>       Labels	Text label for each segment ('\n' allowed).
	 *					Use %1 as a placeholder for the current value.
	 */
	propSet.add( YProperty( YUIProperty_Values,		YOtherProperty	 ) );
	propSet.add( YProperty( YUIProperty_Labels,		YOtherProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YBarGraph::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Values )	return false; // Needs special handling
    else if ( propertyName == YUIProperty_Labels )	return false; // Needs special handling
    else
    {
	YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special handling necessary
}


YPropertyValue
YBarGraph::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Values	)	return YPropertyValue( YOtherProperty );
    else if ( propertyName == YUIProperty_Labels	)	return YPropertyValue( YOtherProperty );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}




YBarGraphMultiUpdate::YBarGraphMultiUpdate( YBarGraph * barGraph )
    : _barGraph ( barGraph )
{
    YUI_CHECK_PTR( barGraph );

    _barGraph->priv->postponeUpdates = true;
}


YBarGraphMultiUpdate::~YBarGraphMultiUpdate()
{
    _barGraph->priv->postponeUpdates = false;

    if ( _barGraph->priv->updatesPending )
	_barGraph->updateDisplay();
}
