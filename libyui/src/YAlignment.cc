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

  File:		YAlignment.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YAlignment.h"
#include "YBothDim.h"
#include "YPath.h"
#include "Libyui_config.h"

using std::string;


struct YAlignmentPrivate
{
    /**
     * Constructor.
     **/
    YAlignmentPrivate( YAlignmentType horAlign,
		       YAlignmentType vertAlign )
	: leftMargin(0)
	, rightMargin(0)
	, topMargin(0)
	, bottomMargin(0)
	, minWidth(0)
	, minHeight(0)
	{
	    alignment.hor  = horAlign;
	    alignment.vert = vertAlign;
	}

    //
    // Data Members
    //

    int leftMargin;
    int rightMargin;
    int topMargin;
    int bottomMargin;

    int minWidth;
    int minHeight;

    string backgroundPixmap;

    YBothDim<YAlignmentType> alignment;
};




YAlignment::YAlignment( YWidget * 	parent,
			YAlignmentType 	horAlign,
			YAlignmentType 	vertAlign )
    : YSingleChildContainerWidget( parent )
    , priv( new YAlignmentPrivate( horAlign, vertAlign ) )
{
    YUI_CHECK_NEW( priv );
}


YAlignment::~YAlignment()
{
    // NOP
}


YAlignmentType
YAlignment::alignment( YUIDimension dim ) const
{
    return priv->alignment[ dim ];
}


int YAlignment::leftMargin() const
{
    return priv->leftMargin;
}


int YAlignment::rightMargin() const
{
    return priv->rightMargin;
}


int YAlignment::topMargin() const
{
    return priv->topMargin;
}


int YAlignment::bottomMargin() const
{
    return priv->bottomMargin;
}


void YAlignment::setLeftMargin( int margin )
{
    priv->leftMargin = margin;
}


void YAlignment::setRightMargin( int margin )
{
    priv->rightMargin = margin;
}


void YAlignment::setTopMargin( int margin )
{
    priv->topMargin = margin;
}


void YAlignment::setBottomMargin( int margin )
{
    priv->bottomMargin = margin;
}


int YAlignment::minWidth() const
{
    return priv->minWidth;
}


int YAlignment::minHeight() const
{
    return priv->minHeight;
}


void YAlignment::setMinWidth( int width )
{
    priv->minWidth = width;
}


void YAlignment::setMinHeight( int height )
{
    priv->minHeight = height;
}


string YAlignment::backgroundPixmap() const
{
    return priv->backgroundPixmap;
}


void YAlignment::addChild( YWidget * child )
{
    YSingleChildContainerWidget::addChild( child );

    if ( minWidth()  > 0 )	child->setStretchable( YD_HORIZ, true );
    if ( minHeight() > 0 )	child->setStretchable( YD_VERT , true );
}


bool YAlignment::stretchable( YUIDimension dim ) const
{
    if ( alignment( dim ) == YAlignUnchanged && hasChildren() )
	return firstChild()->stretchable( dim );
    else
	return true;
}


int YAlignment::preferredWidth()
{
    if ( ! hasChildren() )
	return minWidth();

    int preferredWidth = firstChild()->preferredWidth();
    preferredWidth    += leftMargin() + rightMargin();

    return std::max( minWidth(), preferredWidth );
}


int YAlignment::preferredHeight()
{
    if ( ! hasChildren() )
	return minHeight();

    int preferredHeight = firstChild()->preferredHeight();
    preferredHeight    += topMargin() + bottomMargin();

    return std::max( minHeight(), preferredHeight );
}


void YAlignment::setSize( int newWidth, int newHeight )
{
    if ( ! hasChildren() )
    {
	yuiError() << "No child in " << this << endl;
	return;
    }


    YBothDim<int> newSize;
    newSize.hor  = newWidth;
    newSize.vert = newHeight;

    YBothDim<int> offset;
    offset.hor  = leftMargin();
    offset.vert = topMargin();

    YBothDim<int> totalMargin;
    totalMargin.hor  = leftMargin() + rightMargin();
    totalMargin.vert = topMargin()  + bottomMargin();

    YBothDim<int> newChildSize;
    YBothDim<int> newChildPos;

    YUIDimension dim = YD_HORIZ;
    while ( true ) // only toggle
    {
	int childPreferredSize = firstChild()->preferredSize( dim );
	int preferredSize      = childPreferredSize + totalMargin[ dim ];

	if ( newSize[ dim ] >= preferredSize )
	    // Optimum case: enough space for the child and all margins
	{
	    if ( firstChild()->stretchable( dim ) &&
		 ( alignment( dim ) == YAlignUnchanged ||
		   stretchable( dim ) ) )	// special case: promote child stretchability if Opt(:?stretch) set
	    {
		newChildSize[ dim ] = newSize[ dim ] - totalMargin[ dim ];
	    }
	    else
	    {
		newChildSize[ dim ] = childPreferredSize;
	    }
	}
	else if ( newSize[ dim ] >= childPreferredSize )
	    // Still enough space for the child, but not for all margins
	{
	    newChildSize[ dim ] = childPreferredSize; // Give the child as much space as it needs

	    // Reduce the margins

	    if ( totalMargin[ dim ] > 0 ) // Prevent division by zero
	    {
		// Redistribute remaining space according to margin ratio
		// (disregarding integer rounding errors - we don't care about one pixel)

		int remaining = newSize[ dim ] - childPreferredSize;
		offset     [ dim ] = remaining * offset[ dim ] / totalMargin[ dim ];
		totalMargin[ dim ] = remaining;
	    }

	}
	else // Not even enough space for the child - forget about the margins
	{
	    newChildSize[ dim ] = newSize[ dim ];
	    offset	[ dim ] = 0;
	    totalMargin [ dim ] = 0;
	}


	switch ( alignment( dim ) )
	{
	    case YAlignCenter:
		newChildPos[ dim ] = ( newSize[ dim ] - newChildSize[ dim ] - totalMargin[ dim ] ) / 2;
		break;

	    case YAlignUnchanged:
	    case YAlignBegin:
		newChildPos[ dim ] = 0;
		break;

	    case YAlignEnd:
		newChildPos[ dim ] = newSize[ dim ] - newChildSize[ dim ] - totalMargin[ dim ];
		break;
	}

	newChildPos[ dim ] += offset[ dim ];

	// we need to get out of this loop after the second run
	if (dim == YD_HORIZ)
	  dim = YD_VERT;
	else
	  break;
    }

    firstChild()->setSize( newChildSize.hor, newChildSize.vert );
    moveChild( firstChild(), newChildPos.hor, newChildPos.vert );

#if 0
    yuiDebug() << "setSize( alignment, " << newWidth         << ", " << newHeight         << ")" << endl;
    yuiDebug() << "setSize( child, "     << newChildSize.hor << ", " << newChildSize.vert << ")" << endl;
    yuiDebug() << "moveChild( "          << newChildPos.hor  << ", " << newChildPos.vert  << ")" << endl;
#endif
}



int YAlignment::totalMargins( YUIDimension dim ) const
{
    if ( dim == YD_HORIZ )	return leftMargin() + rightMargin();
    else			return topMargin()  + bottomMargin();
}



void YAlignment::setBackgroundPixmap( const string & pixmapFileName )
{
    string pixmap = pixmapFileName;

    if ( pixmap.length() > 0 &&
	 pixmap[0] != '/'  &&	// Absolute path?
	 pixmap[0] != '.'    )	// Path relative to $CWD ?
    {
	// Prepend theme dir

	YPath pix( THEMEDIR, pixmap );

	pixmap = pix.path();
    }

    priv->backgroundPixmap = pixmap;
}


const char *
YAlignment::widgetClass() const
{
    static const char *YAlignment_classes[3][5] =
    {
        {"YAlignment_Left", "YAlignment_HCenter", "YAlignment_Right",  "YMarginBox", "YMinWidth"},
        {"YAlignment_Top",  "YAlignment_VCenter", "YAlignment_Bottom", "YMarginBox", "YMinHeight"},
        {0,                 "YAlignment_HVCenter", 0,                  "YAlignment", "YMinSize"},
    };

    int hIndex = 3;
    int vIndex = 2;

    if      ( priv->alignment.hor == YAlignBegin    ) { vIndex = 0; hIndex = 0; }
    else if ( priv->alignment.hor == YAlignEnd      ) { vIndex = 0; hIndex = 2; }
    else if ( priv->alignment.hor == YAlignCenter   )
    {
        vIndex = 0; hIndex = 1;
        if  ( priv->alignment.vert == YAlignCenter  )
            vIndex = 2;
    }
    else if ( priv->alignment.vert == YAlignBegin   ) { vIndex = 1; hIndex = 0; }
    else if ( priv->alignment.vert == YAlignEnd     ) { vIndex = 1; hIndex = 2; }
    else if ( priv->alignment.vert == YAlignCenter  ) { vIndex = 1; hIndex = 1; }

    if ( priv->alignment.hor  == YAlignUnchanged &&
         priv->alignment.vert == YAlignUnchanged )
    {
        if ( priv->leftMargin   > 0 ||
             priv->rightMargin  > 0 ||
             priv->topMargin    > 0 ||
             priv->bottomMargin > 0 )
        {
            vIndex = 0; hIndex = 3;
        }

        if ( priv->minWidth > 0 || priv->minHeight > 0 )
        {
            if      ( priv->minWidth  == 0 ) { vIndex = 1; hIndex = 4; }
            else if ( priv->minHeight == 0 ) { vIndex = 0; hIndex = 4; }
            else                             { vIndex = 2; hIndex = 4; }
        }
    }
    return YAlignment_classes[vIndex][hIndex];
}
