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

  File:		YButtonBox.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <algorithm>	// max()
#include <vector>
#include <list>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YButtonBox.h"
#include "YPushButton.h"
#include "YUI.h"
#include "YApplication.h"

using std::string;
using std::vector;


YButtonBoxLayoutPolicy	YButtonBox::_layoutPolicy = kdeLayoutPolicy();
YButtonBoxMargins	YButtonBox::_defaultMargins;


struct YButtonBoxPrivate
{
    /**
     * Constructor
     **/
    YButtonBoxPrivate()
	: sanityCheckRelaxed( false )
	, margins( YButtonBox::_defaultMargins )
	{}

    //
    // Data members
    //

    bool		sanityCheckRelaxed;
    YButtonBoxMargins	margins;
};




YButtonBox::YButtonBox( YWidget * parent )
    : YWidget( parent )
    , priv( new YButtonBoxPrivate() )
{
    YUI_CHECK_NEW( priv );
    setChildrenManager( new YWidgetChildrenManager( this ) );
}


YButtonBox::~YButtonBox()
{
    // NOP
}


void
YButtonBox::setLayoutPolicy( const YButtonBoxLayoutPolicy & layoutPolicy )
{
    _layoutPolicy = layoutPolicy;
}


YButtonBoxLayoutPolicy
YButtonBox::layoutPolicy()
{
    return _layoutPolicy;
}


YButtonBoxLayoutPolicy
YButtonBox::kdeLayoutPolicy()
{
    YButtonBoxLayoutPolicy policy;

    policy.buttonOrder           = YKDEButtonOrder;
    policy.equalSizeButtons      = false;
    policy.alignment[ YD_HORIZ ] = YAlignCenter;
    policy.alignment[ YD_VERT  ] = YAlignBegin;	// Align top

    return policy;
}


YButtonBoxLayoutPolicy
YButtonBox::gnomeLayoutPolicy()
{
    YButtonBoxLayoutPolicy policy;

    policy.buttonOrder           = YGnomeButtonOrder;
    policy.equalSizeButtons      = true;
    policy.alignment[ YD_HORIZ ] = YAlignEnd;	// Align right
    policy.alignment[ YD_VERT  ] = YAlignBegin;	// Align top
    policy.addExcessSpaceToHelpButtonExtraMargin = true;

    return policy;
}


void
YButtonBox::setDefaultMargins( const YButtonBoxMargins & margins )
{
    _defaultMargins = margins;
}


YButtonBoxMargins
YButtonBox::defaultMargins()
{
    return _defaultMargins;
}


void
YButtonBox::setMargins( const YButtonBoxMargins & margins )
{
    priv->margins = margins;
}


YButtonBoxMargins
YButtonBox::margins() const
{
    return priv->margins;
}


void
YButtonBox::setSize( int newWidth, int newHeight )
{
    sanityCheck();
    doLayout( newWidth, newHeight );
}


void
YButtonBox::doLayout( int width, int height )
{
    vector<YPushButton *> buttons = buttonsByButtonOrder();

    if ( buttons.empty() )
	return;

    YPushButton * helpButton = findButton( YHelpButton );

    int prefWidth  = preferredWidth();
    int prefHeight = preferredHeight();
    YButtonBoxMargins margins = priv->margins;
    bool equalSizeButtons = _layoutPolicy.equalSizeButtons;


    //
    // Horizontal layout
    //

    if ( width < prefWidth ) // Not enough horizontal space
    {
	if ( equalSizeButtons )
	{
	    int buttonWidthWithoutMargins = maxChildSize( YD_HORIZ ) * buttons.size();

	    if ( width < buttonWidthWithoutMargins )
	    {
		// The missing width can't be compensated by reducing margins and spacings.
		// Try not enforcing the same width:
		//
		// If one button is very much larger than most others, that one
		// button will greatly distort the overall layout. If we simply cut
		// some pixels off every button, for sure that one very large
		// button will become unreadable. So let's try first with buttons
		// getting just the size they really need.
		//
		// Of course, we might still have cut some pixels off all buttons
		// if that also is too wide, but in that case we can't do very much
		// anyway.

		equalSizeButtons = false;
		prefWidth = preferredWidth( equalSizeButtons );
	    }
	}
    }

    int widthLoss = 0;

    if ( width < prefWidth ) // Not enough horizontal space
    {
	// Try reducing margins

	int missing = prefWidth - width;

	if ( missing <= margins.left + margins.right )
	{
	    margins.left  -= missing / 2;
	    margins.right -= missing / 2;
	    missing = 0;
	}
	else
	{
	    missing -= margins.left;
	    missing -= margins.right;
	    margins.left  = 0;
	    margins.right = 0;
	}

	if ( missing > 0 && buttons.size() > 1 )
	{
	    // Try reducing spacing

	    int totalSpacing = ( buttons.size() - 1 ) * margins.spacing;

	    if ( missing <= totalSpacing )
	    {
		totalSpacing -= missing;
		margins.spacing = totalSpacing / ( buttons.size() - 1 );
		missing = 0;
	    }
	    else
	    {
		missing -= totalSpacing;
		margins.spacing = 0;
	    }
	}

	if ( missing > 0 && helpButton )
	{
	    // Try reducing help button extra spacing

	    if ( missing <= margins.helpButtonExtraSpacing )
	    {
		margins.helpButtonExtraSpacing -= missing;
		missing = 0;
	    }
	    else
	    {
		missing -= margins.helpButtonExtraSpacing;
		margins.helpButtonExtraSpacing = 0;
	    }
	}


	// Distribute missing width among all buttons

	if ( missing > 0 )
	    widthLoss = missing / buttons.size();
    }

    if ( width > prefWidth ) // Excess horizontal space
    {
	int excessWidth = width - prefWidth;

	if ( _layoutPolicy.addExcessSpaceToHelpButtonExtraMargin && helpButton )
	{
	    margins.helpButtonExtraSpacing += excessWidth;
	}
	else
	{
	    switch ( _layoutPolicy.alignment[ YD_HORIZ ] )
	    {
		case YAlignCenter:
		    margins.left  += excessWidth / 2;
		    margins.right += excessWidth / 2;
		    break;

		case YAlignBegin:
		case YAlignUnchanged:
		    margins.right += excessWidth;
		    break;

		case YAlignEnd:
		    margins.left  += excessWidth;
		    break;
	    }
	}
    }


    //
    // Vertical layout
    //

    int buttonHeight = maxChildSize( YD_VERT );

    if ( height < prefHeight ) // Not enough vertical space
    {
	// Reduce margins

	int missing = prefHeight - height;

	if ( missing < margins.top + margins.bottom )
	{
	    margins.top    -= missing / 2;
	    margins.bottom -= missing / 2;
	}
	else
	{
	    margins.top    = 0;
	    margins.bottom = 0;
	}
    }

    if ( height < buttonHeight )
    {
	buttonHeight = height;
    }

    int y_pos = margins.top;

    if ( height > prefHeight ) // Excess vertical space
    {
	// Distribute excess vertical space

	int excessHeight = height - buttonHeight;
	excessHeight -= margins.top;
	excessHeight -= margins.bottom;

	switch ( _layoutPolicy.alignment[ YD_VERT ] )
	{
	    case YAlignBegin:		// Align top
	    case YAlignUnchanged:
		break;

	    case YAlignCenter:
		y_pos += excessHeight / 2;
		break;

	    case YAlignEnd:		// Align bottom
		y_pos += excessHeight;
		break;
	}
    }


    //
    // Set child widget positions and sizes from left to right
    //

    int x_pos        = margins.left;
    int buttonWidth  = 0;

    if ( equalSizeButtons )
    {
	buttonWidth  = maxChildSize( YD_HORIZ );
	buttonWidth -= widthLoss;
    }

    bool reverseLayout = YUI::app()->reverseLayout();

    for ( vector<YPushButton *>::iterator it = buttons.begin();
	  it != buttons.end();
	  ++it )
    {
	YPushButton * button = *it;

	// Extra spacing left of [Help] button
	// (Only if this isn't the first button)

	if ( button == helpButton && button != buttons.front() )
	    x_pos += margins.helpButtonExtraSpacing;

	if ( ! equalSizeButtons )
	{
	    buttonWidth  = button->preferredWidth();
	    buttonWidth -= widthLoss;
	}

	button->setSize( buttonWidth, buttonHeight );

	if ( reverseLayout )
	    moveChild( button, width - x_pos - buttonWidth, y_pos );
	else
	    moveChild( button, x_pos, y_pos );

	x_pos += buttonWidth;
	x_pos += margins.spacing;


	// Extra spacing right of [Help] button

	if ( button == helpButton )
	    x_pos += margins.helpButtonExtraSpacing;
    }
}


vector<YPushButton *>
YButtonBox::buttonsByButtonOrder()
{
    vector<YPushButton *> specialButtons( YMaxButtonRole, (YPushButton *) 0 );
    vector<YPushButton *> customButtons;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( ! button )
	    YUI_THROW( YUIInvalidChildException<YWidget>( this, *it ) );

	switch ( button->role() )
	{
	    case YOKButton:
	    case YCancelButton:
	    case YApplyButton:
	    case YHelpButton:
	    case YRelNotesButton:

		if ( specialButtons[ button->role() ] ) // Only one of each of those is allowed
		{
		    string msg = "Multiple buttons with that role [";
		    msg += button->debugLabel();
		    msg += "]";
		    YUI_THROW( YUIButtonRoleMismatchException( msg ) );
		}
		else
		{
		    specialButtons[ button->role() ] = button;
		}
		break;

	    case YCustomButton:
		customButtons.push_back( button );
		break;

	    case YMaxButtonRole:
		YUI_THROW( YUIButtonRoleMismatchException( "Invalid button role" ) );
		break;
	}
    }

    vector<YPushButton *> buttons;

    if ( _layoutPolicy.buttonOrder == YKDEButtonOrder )
    {
	if ( specialButtons[ YOKButton     ] )	buttons.push_back( specialButtons[ YOKButton     ] );
	if ( specialButtons[ YApplyButton  ] )	buttons.push_back( specialButtons[ YApplyButton  ] );
	if ( specialButtons[ YCancelButton ] )	buttons.push_back( specialButtons[ YCancelButton ] );

	buttons.insert( buttons.end(), customButtons.begin(), customButtons.end() );

	if ( specialButtons[ YHelpButton   ] )	buttons.push_back( specialButtons[ YHelpButton   ] );
    }
    else // YGnomeButtonOrder
    {
	if ( specialButtons[ YHelpButton   ] )	buttons.push_back( specialButtons[ YHelpButton   ] );

	buttons.insert( buttons.end(), customButtons.begin(), customButtons.end() );

	if ( specialButtons[ YApplyButton  ] )	buttons.push_back( specialButtons[ YApplyButton  ] );
	if ( specialButtons[ YCancelButton ] )	buttons.push_back( specialButtons[ YCancelButton ] );
	if ( specialButtons[ YOKButton     ] )	buttons.push_back( specialButtons[ YOKButton     ] );
    }

    return buttons;
}



int
YButtonBox::preferredWidth( bool equalSizeButtons )
{
    if ( childrenCount() < 1 )
	return 0;

    int width = ( childrenCount() - 1 ) * priv->margins.spacing;

    if ( equalSizeButtons )
	width += maxChildSize( YD_HORIZ ) * childrenCount();
    else
	width += totalChildrenWidth();

    width += priv->margins.left;
    width += priv->margins.right;

    if ( priv->margins.helpButtonExtraSpacing )
    {
	if ( findButton( YHelpButton ) )
	    width += priv->margins.helpButtonExtraSpacing;
    }

    return width;
}


int
YButtonBox::preferredWidth()
{
    return preferredWidth( _layoutPolicy.equalSizeButtons );
}


int
YButtonBox::preferredHeight()
{
    int height = maxChildSize( YD_VERT );
    height += priv->margins.top;
    height += priv->margins.bottom;

    return height;
}


int
YButtonBox::maxChildSize( YUIDimension dim ) const
{
    int maxSize = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	maxSize = std::max( maxSize, (*it)->preferredSize( dim ) );
    }

    return maxSize;
}


int
YButtonBox::totalChildrenWidth() const
{
    int totalWidth = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	totalWidth += (*it)->preferredWidth();
    }

    return totalWidth;
}


bool
YButtonBox::stretchable( YUIDimension dimension ) const
{
    switch ( dimension )
    {
	case YD_HORIZ:	return true;
	case YD_VERT :	return false;

	default:
	    YUI_THROW( YUIInvalidDimensionException() );
	    return 0;
    }
}


YPushButton *
YButtonBox::findButton( YButtonRole role )
{
    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( button && button->role() == role )
	    return button;
    }

    return 0;
}


void
YButtonBox::setSanityCheckRelaxed( bool relaxed )
{
    priv->sanityCheckRelaxed = relaxed;
}


bool
YButtonBox::sanityCheckRelaxed() const
{
    return priv->sanityCheckRelaxed;
}


void
YButtonBox::sanityCheck()
{
    YPushButton * okButton     = 0;
    YPushButton * cancelButton = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YPushButton * button = dynamic_cast<YPushButton *> (*it);

	if ( ! button )
	    YUI_THROW( YUIInvalidChildException<YWidget>( this, *it ) );

	switch ( button->role() )
	{
	    case YOKButton:

		if ( okButton )
		    YUI_THROW( YUIButtonRoleMismatchException( "Multiple buttons with role [OK]" ) );
		else
		    okButton = button;
		break;


	    case YCancelButton:

		if ( cancelButton )
		    YUI_THROW( YUIButtonRoleMismatchException( "Multiple buttons with role [Cancel]" ) );
		else
		    cancelButton = button;
		break;


	    default:
		break;
	}
    }

    if ( childrenCount() > 1 && ! sanityCheckRelaxed() )
    {
	if ( ! okButton || ! cancelButton )
	    YUI_THROW( YUIButtonRoleMismatchException( "Button role mismatch: Must have both [OK] and [Cancel] roles" ) );
    }
}
