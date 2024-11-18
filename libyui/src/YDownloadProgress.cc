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

  File:		YDownloadProgress.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <sys/stat.h>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YDownloadProgress.h"

using std::string;
	

struct YDownloadProgressPrivate
{
    YDownloadProgressPrivate( const string &	label,
			      const string &	filename,
			      YFileSize_t	expectedSize )
	: label( label )
	, filename( filename )
	, expectedSize( expectedSize )
	{}

    string	label;
    string	filename;
    YFileSize_t	expectedSize;
};


YDownloadProgress::YDownloadProgress( YWidget *		parent,
				      const string &	label,
				      const string &	filename,
				      YFileSize_t	expectedSize )
    : YWidget( parent )
    , priv( new YDownloadProgressPrivate( label, filename, expectedSize ) )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT, false );
}


YDownloadProgress::~YDownloadProgress()
{
    // NOP
}


string
YDownloadProgress::label() const
{
    return priv->label;
}


void
YDownloadProgress::setLabel( const string & label )
{
    priv->label = label;
}


string
YDownloadProgress::filename() const
{
    return priv->filename;
}


void
YDownloadProgress::setFilename( const string & filename )
{
    priv->filename = filename;
}


YFileSize_t
YDownloadProgress::expectedSize() const
{
    return priv->expectedSize;
}


void
YDownloadProgress::setExpectedSize( YFileSize_t newSize )
{
    priv->expectedSize = newSize;
}


int
YDownloadProgress::currentPercent() const
{
    if ( priv->expectedSize == 0 )	// Avoid division by zero
	return 0;

    YFileSize_t currentSize = currentFileSize();

    if ( currentSize >= priv->expectedSize )
	return 100;
    else
	return (int) ( (100 * currentSize ) / priv->expectedSize );
}


YFileSize_t
YDownloadProgress::currentFileSize() const
{
    struct stat stat_info;

    if ( stat( priv->filename.c_str(), & stat_info ) == 0 )
	return (YFileSize_t) stat_info.st_size;
    else
	return 0;
}


const YPropertySet &
YDownloadProgress::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
	/*
	 * @property string	Label		text above the progress bar
	 * @property string	Filename	name of the file that is monitored
	 * @property integer	ExpectedSize	expected size of the file in bytes
	 * @property integer	CurrentSize	current  size of the file in bytes (read-only!)
	 * @property integer	Value		current  percent of the download   (read-only!)
	 */
	propSet.add( YProperty( YUIProperty_Label, 		YStringProperty  ) );
	propSet.add( YProperty( YUIProperty_Filename,		YStringProperty  ) );
	propSet.add( YProperty( YUIProperty_ExpectedSize,	YIntegerProperty ) );
	propSet.add( YProperty( YUIProperty_CurrentSize,	YIntegerProperty, true ) ); // read-only
	propSet.add( YProperty( YUIProperty_Value,		YIntegerProperty, true ) ); // read-only
	propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YDownloadProgress::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if ( propertyName == YUIProperty_Label 		)	setLabel       ( val.stringVal()  );
    if ( propertyName == YUIProperty_Filename		)	setFilename    ( val.stringVal()  );
    if ( propertyName == YUIProperty_ExpectedSize	)	setExpectedSize( val.integerVal() );
    else
    {
	YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special handling necessary
}


YPropertyValue
YDownloadProgress::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if ( propertyName == YUIProperty_Label 		)	return YPropertyValue( label() 		);
    if ( propertyName == YUIProperty_Filename		)	return YPropertyValue( filename() 	);
    if ( propertyName == YUIProperty_ExpectedSize	)	return YPropertyValue( expectedSize() 	);
    if ( propertyName == YUIProperty_CurrentSize	)	return YPropertyValue( currentFileSize());
    if ( propertyName == YUIProperty_Value		)	return YPropertyValue( currentPercent() );
    else
    {
	return YWidget::getProperty( propertyName );
    }
}
