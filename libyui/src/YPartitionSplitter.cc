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

  File:		YPartitionSplitter.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YPartitionSplitter.h"

using std::string;


struct YPartitionSplitterPrivate
{
    YPartitionSplitterPrivate( int		usedSize,
			       int		totalFreeSize,
			       int		minNewPartSize,
			       int		minFreeSize,
			       const string &	usedLabel,
			       const string &	freeLabel,
			       const string &	newPartLabel,
			       const string &	freeFieldLabel,
			       const string &	newPartFieldLabel )
	: usedSize		( usedSize		)
	, totalFreeSize		( totalFreeSize		)
	, minNewPartSize	( minNewPartSize	)
	, minFreeSize		( minFreeSize		)
	, usedLabel		( usedLabel		)
	, freeLabel		( freeLabel		)
	, newPartLabel		( newPartLabel		)
	, freeFieldLabel	( freeFieldLabel	)
	, newPartFieldLabel	( newPartFieldLabel	)
	{}

    int 	usedSize;
    int 	totalFreeSize;
    int 	minNewPartSize;
    int 	minFreeSize;
    string	usedLabel;
    string	freeLabel;
    string	newPartLabel;
    string	freeFieldLabel;
    string	newPartFieldLabel;
};




YPartitionSplitter::YPartitionSplitter( YWidget *	parent,
					int		usedSize,
					int		totalFreeSize,
					int		newPartSize,
					int		minNewPartSize,
					int		minFreeSize,
					const string &	usedLabel,
					const string &	freeLabel,
					const string &	newPartLabel,
					const string &	freeFieldLabel,
					const string &	newPartFieldLabel )
    : YWidget( parent )
    , priv( new YPartitionSplitterPrivate( usedSize,
					   totalFreeSize,
					   minNewPartSize,
					   minFreeSize,
					   usedLabel,
					   freeLabel,
					   newPartLabel,
					   freeFieldLabel,
					   newPartFieldLabel )
	    )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT, false );
}


YPartitionSplitter::~YPartitionSplitter()
{
    // NOP
}


int YPartitionSplitter::usedSize() const
{
    return priv->usedSize;
}


int YPartitionSplitter::totalFreeSize() const
{
    return priv->totalFreeSize;
}


int YPartitionSplitter::minNewPartSize() const
{
    return priv->minNewPartSize;
}


int YPartitionSplitter::minFreeSize() const
{
    return priv->minFreeSize;
}


string YPartitionSplitter::usedLabel() const
{
    return priv->usedLabel;
}


string YPartitionSplitter::freeLabel() const
{
    return priv->freeLabel;
}


string YPartitionSplitter::newPartLabel() const
{
    return priv->newPartLabel;
}


string YPartitionSplitter::freeFieldLabel() const
{
    return priv->freeFieldLabel;
}


string YPartitionSplitter::newPartFieldLabel() const
{
    return priv->newPartFieldLabel;
}


const YPropertySet &
YPartitionSplitter::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property string	Value	the value (the size of the new partition)
	 */
	propSet.add( YProperty( YUIProperty_Value,		YStringProperty	 ) );
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YPartitionSplitter::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if	    ( propertyName == YUIProperty_Value		)	setValue( val.integerVal() );
    else
    {
	return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YPartitionSplitter::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if	    ( propertyName == YUIProperty_Value		)	return YPropertyValue( value() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
