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

  File:		YImage.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include "YImage.h"
#include "YBothDim.h"

using std::string;


struct YImagePrivate
{
    /**
     * Constructor.
     **/
    YImagePrivate( const string & imageFileName, bool animated )
	: imageFileName( imageFileName )
	, animated( animated )
	, autoScale( false )
    {
	zeroSize.hor  = false;
	zeroSize.vert = false;
    }


    string		imageFileName;
    bool		animated;
    YBothDim<bool>	zeroSize;
    bool		autoScale;
};




YImage::YImage( YWidget *		parent,
		const string &	imageFileName,
		bool			animated )
    : YWidget( parent )
    , priv( new YImagePrivate( imageFileName, animated ) )
{
    YUI_CHECK_NEW( priv );
}


YImage::~YImage()
{
    // NOP
}


string YImage::imageFileName() const
{
    return priv->imageFileName;
}


bool YImage::animated() const
{
    return priv->animated;
}


void YImage::setImage( const string & imageFileName, bool animated )
{
    priv->imageFileName = imageFileName;
    priv->animated	= animated;
}


bool YImage::hasZeroSize( YUIDimension dim ) const
{
    return priv->zeroSize[ dim ];
}


void YImage::setZeroSize( YUIDimension dim, bool zeroSize )
{
    priv->zeroSize[ dim ] = zeroSize;
    setStretchable( dim, zeroSize );
}


bool YImage::autoScale() const
{
    return priv->autoScale;
}


void YImage::setAutoScale( bool autoScale )
{
    priv->autoScale = autoScale;
}
