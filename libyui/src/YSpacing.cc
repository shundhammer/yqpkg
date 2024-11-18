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

  File:		YEmpty.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include "YSpacing.h"
#include "YUI.h"
#include "YApplication.h"


struct YSpacingPrivate
{
    YSpacingPrivate( YUIDimension dim, int size )
	: dim( dim )
	, size( size )
	{}

    YUIDimension	dim;
    int			size;
};




YSpacing::YSpacing( YWidget * parent, YUIDimension dim, bool stretchable, YLayoutSize_t layoutUnits )
    : YWidget( parent )
    , priv( new YSpacingPrivate( dim, YUI::app()->deviceUnits( dim, layoutUnits ) ) )
{
    YUI_CHECK_NEW( priv );
    setStretchable( dim, stretchable );
    setStretchable( dim == YD_HORIZ ? YD_VERT : YD_HORIZ, false );
}


YSpacing::~YSpacing()
{
    // NOP
}


YUIDimension
YSpacing::dimension() const
{
    return priv->dim;
}


int YSpacing::size() const
{
    return priv->size;
}


int YSpacing::size( YUIDimension dim ) const
{
    if ( dim == priv->dim )	return priv->size;
    else			return 0;
}


int YSpacing::preferredWidth()
{
    if ( priv->dim == YD_HORIZ )
	return priv->size;
    else
	return 0;
}


int YSpacing::preferredHeight()
{
    if ( priv->dim == YD_VERT )
	return priv->size;
    else
	return 0;
}


const char *
YSpacing::widgetClass() const
{
    if ( priv->size > 0 )
	return ( priv->dim == YD_HORIZ ) ? "YHSpacing" : "YVSpacing";

    if ( stretchable( YD_HORIZ ) )	return "YHStretch";
    if ( stretchable( YD_VERT  ) )	return "YVStretch";

    return "YSpacing";
}
