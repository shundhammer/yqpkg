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

  File:		YGraph.cc

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/


#define YUILogComponent "ui-graph"
#include "YUILog.h"

#include "YGraph.h"

using std::string;


struct YGraphPrivate
{
    YGraphPrivate( string filename, string layoutAlgorithm )
	: filename( filename ),
	  layoutAlgorithm( layoutAlgorithm )
	{}

    string filename;
    string layoutAlgorithm;
};


YGraph::YGraph( YWidget * parent, const string & filename, const string & layoutAlgorithm )
    : YWidget( parent )
    , priv( new YGraphPrivate( filename, layoutAlgorithm ) )
{
    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YGraph::YGraph( YWidget * parent, /* graph_t */ void * graph )
    : YWidget( parent )
    , priv( new YGraphPrivate( "", "" ) )
{
    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YGraph::~YGraph()
{
    // NOP
}


string
YGraph::filename() const
{
    return priv->filename;
}


void
YGraph::setFilename( const string & filename )
{
    priv->filename = filename;
    renderGraph( filename, layoutAlgorithm() );
}


string
YGraph::layoutAlgorithm() const
{
    return priv->layoutAlgorithm;
}


void
YGraph::setGraph( /* graph_t */ void * graph )
{
    priv->filename.clear();
    renderGraph( graph );
}


void
YGraph::setLayoutAlgorithm( const string & layoutAlgorithm )
{
    priv->layoutAlgorithm = layoutAlgorithm;
}


string
YGraph::activatedNode() const
{
    return "";
}


const YPropertySet &
YGraph::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property string	Filename	name of the file describing the graph
	 * @property string	Layout		layout-algorithm used from the graph
	 * @property string	Item		activated node (read-only)
	 */
	propSet.add( YProperty( YUIProperty_Filename,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_Layout,		YStringProperty	 ) );
	propSet.add( YProperty( YUIProperty_Item,		YStringProperty, true ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YGraph::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Filename	)	setFilename( val.stringVal() );
    else if ( propertyName == YUIProperty_Layout	)	setLayoutAlgorithm( val.stringVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YGraph::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Filename	)	return YPropertyValue( filename() );
    else if ( propertyName == YUIProperty_Layout	)	return YPropertyValue( layoutAlgorithm() );
    else if ( propertyName == YUIProperty_Item		)	return YPropertyValue( activatedNode() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
