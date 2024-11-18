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

  File:		YLayoutBox.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <iomanip>	// setw()
#include <algorithm>	// max()

#define YUILogComponent "ui-layout"
#include "YUILog.h"

#include "YLayoutBox.h"
#include "YAlignment.h"
#include "YSpacing.h"
#include "YUI.h"
#include "YApplication.h"


struct YLayoutBoxPrivate
{
    /**
     * Constructor
     **/
    YLayoutBoxPrivate( YUIDimension prim )
	: primary( prim )
	, secondary( prim == YD_HORIZ ? YD_VERT : YD_HORIZ )
	, debugLayout( false )
	{}

    //
    // Data members
    //

    YUIDimension	primary;
    YUIDimension 	secondary;
    bool 		debugLayout;
};




YLayoutBox::YLayoutBox( YWidget * parent, YUIDimension primaryDimension )
    : YWidget( parent )
    , priv( new YLayoutBoxPrivate( primaryDimension ) )
{
    YUI_CHECK_NEW( priv );
    setChildrenManager( new YWidgetChildrenManager( this ) );
}


YLayoutBox::~YLayoutBox()
{
    // NOP
}


YUIDimension
YLayoutBox::primary() const
{
    return priv->primary;
}


YUIDimension
YLayoutBox::secondary() const
{
    return priv->secondary;
}


bool
YLayoutBox::debugLayout() const
{
    return priv->debugLayout;
}

void
YLayoutBox::setDebugLayout( bool deb )
{
    priv->debugLayout = deb;

    yuiDebug() << "YLayoutBox: Layout debugging: " << std::boolalpha << deb << endl;
}


int
YLayoutBox::preferredSize( YUIDimension dimension )
{
    if ( dimension == secondary() )	// the easy case first: secondary dimension
    {
	return childrenMaxPreferredSize( dimension );
    }
    else
    {
	/*
	 * In the primary dimension things are much more complicated: We want to
	 * honor any weights specified under all circumstances.  So we first
	 * need to determine the "dominating child" - the widget that determines the
	 * overall size with respect to its weight in that dimension. Once we
	 * know that, we need to stretch all other weighted children accordingly
	 * so the weight ratios are respected.
	 *
	 * As a final step, the preferred sizes of all children that don't have
	 * a weight attached are summed up.
	 */

	int size = 0L;

	// Search for the dominating child
	YWidget * dominatingChild = findDominatingChild();

	if ( dominatingChild )
	{
	    // Calculate size of all weighted widgets.

	    size = dominatingChild->preferredSize( primary() )
		* childrenTotalWeight( primary() )
		/ dominatingChild->weight( primary() );

	    // Maintain this order of calculation in order to minimize integer
	    // rounding errors!
	}


	// Add up the size of all non-weighted children;
	// they will get their respective preferred size.

	size += totalNonWeightedChildrenPreferredSize( primary() );

	return size;
    }
}


int YLayoutBox::preferredWidth()
{
    return preferredSize( YD_HORIZ );
}


int YLayoutBox::preferredHeight()
{
    return preferredSize( YD_VERT );
}


/*
 * Search for the "dominating child" widget.
 *
 * This is the widget that determines the overall size of the
 * container with respect to all children's weights: It is the child
 * with the maximum ratio of preferred size and weight. All other
 * weighted children need to be stretched accordingly so the weight
 * ratios can be maintained.
 *
 * Returns 0 if there is no dominating child, i.e. if there are only
 * non-weighted children.
 */

YWidget *
YLayoutBox::findDominatingChild()
{
    YWidget *	dominatingChild	= 0;
    double 	dominatingRatio	= 0.0;
    double 	ratio;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	YWidget * child = *it;

	if ( child->weight( primary() ) != 0 )	// avoid division by zero
	{
	    ratio = ( ( double ) child->preferredSize( primary() ) )
		/ child->weight( primary() );

	    if ( ratio > dominatingRatio ) // we have a new dominating child
	    {
		dominatingChild = child;
		dominatingRatio = ratio;
	    }
	}
    }


    if ( debugLayout() )
    {
	if ( dominatingChild )
	{
	    yuiDebug() << "Found dominating child: "	<< dominatingChild
		       << " - preferred size: " 	<< dominatingChild->preferredSize( primary() )
		       << ", weight: " 			<< dominatingChild->weight( primary() )
		       << endl;
	}
	else
	{
	    yuiDebug() << "This layout doesn't have a dominating child." << endl;
	}
    }

    return dominatingChild;
}


int
YLayoutBox::childrenMaxPreferredSize( YUIDimension dimension )
{
    int maxPreferredSize = 0L;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	maxPreferredSize = std::max( (*it)->preferredSize( dimension ), maxPreferredSize );
    }

    return maxPreferredSize;
}


int
YLayoutBox::childrenTotalWeight( YUIDimension dimension )
{
    int totalWeight = 0L;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	totalWeight += (*it)->weight( dimension );
    }

    return totalWeight;
}


int
YLayoutBox::totalNonWeightedChildrenPreferredSize( YUIDimension dimension )
{
    int size = 0L;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	if ( ! (*it)->hasWeight( dimension ) ) // non-weighted children only
	    size += (*it)->preferredSize( dimension );
    }

    return size;
}


int
YLayoutBox::countNonWeightedChildren( YUIDimension dimension )
{
    int count = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	if ( ! (*it)->hasWeight( dimension ) )
	    count++;
    }

    return count;
}


int
YLayoutBox::countStretchableChildren( YUIDimension dimension )
{
    int count = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	if ( ! (*it)->hasWeight( dimension ) &&
	     (*it)->stretchable( dimension ) )
	    count++;
    }

    return count;
}


int
YLayoutBox::countLayoutStretchChildren( YUIDimension dimension )
{
    int count = 0;

    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	if ( ! (*it)->hasWeight( dimension ) &&
	     isLayoutStretch( *it, dimension ) )
	    count++;
    }

    return count;
}


bool
YLayoutBox::isLayoutStretch( YWidget * child, YUIDimension dimension )
{
    if ( ! child )
	return false;

    YSpacing * spacing = dynamic_cast<YSpacing *> (child);

    if ( spacing && spacing->stretchable( dimension ) )
	return true;
    else
	return false;
}



bool
YLayoutBox::stretchable( YUIDimension dimension ) const
{
    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it )
    {
	if ( (*it)->stretchable( dimension ) ||
	     (*it)->hasWeight( dimension ) )
	    return true;
    }

    return false;
}


void
YLayoutBox::setSize( int newWidth, int newHeight )
{
    int count = childrenCount();
    sizeVector	widths	( count );
    sizeVector	heights ( count );
    posVector	x_pos	( count );
    posVector	y_pos	( count );

    if ( primary() == YD_HORIZ )
    {
	calcPrimaryGeometry  ( newWidth,  widths,  x_pos );
	calcSecondaryGeometry( newHeight, heights, y_pos );
    }
    else
    {
	calcPrimaryGeometry  ( newHeight, heights, y_pos );
	calcSecondaryGeometry( newWidth,  widths,  x_pos );
    }

    if ( YUI::app()->reverseLayout() )
    {
	// Mirror the widget X geometry for languages with left-to-right
	// writing direction (Arabic, Hebrew).

	for ( int i = 0; i < childrenCount(); i++ )
	    x_pos[i] = newWidth - x_pos[i] - widths[i];
    }

    doResize( widths, heights, x_pos, y_pos );
}


void
YLayoutBox::calcPrimaryGeometry( int		newSize,
				 sizeVector &	childSize,
				 posVector  &	childPos )
{
    int pos = 0L;
    int distributableSize = newSize - totalNonWeightedChildrenPreferredSize( primary() );

    if ( distributableSize >= 0L )
    {
	// The (hopefully) normal case: There is enough space.
	// The non-weighted children will get their preferred sizes,
	// the rest will be distributed among the weighted children
	// according to their respective weight ratios.

	int nonWeightedExtra 	= 0L;
	int totalWeight	= childrenTotalWeight( primary() );
	int  rubberBands	= 0;
	int rubberBandExtra	= 0L;

	if ( totalWeight <= 0 )
	{
	    // If there are no weighted children, equally divide the
	    // extra space among the stretchable children (if any).
	    // This includes any layout stretch spaces.

	    int stretchableChildren = countStretchableChildren( primary() );

	    if ( stretchableChildren > 0 )	// avoid division by zero
		nonWeightedExtra = distributableSize / stretchableChildren;
	}
	else
	{
	    // If there are weighted children and there are rubber band
	    // widgets, equally divide any surplus space (i.e. space that
	    // exceeds the weighted children's preferred sizes with respect to
	    // their weights) between the rubber bands.
	    //
	    // This offers an easy way to make nicely even spaced buttons
	    // of equal size: Give all buttons a weight of 1 and insert a
	    // stretch (without weight!) between each.

	    int surplusSize = newSize - preferredSize( primary() );

	    if ( surplusSize > 0L )
	    {
		rubberBands = countLayoutStretchChildren( primary() );

		if ( rubberBands > 0 )
		{
		    rubberBandExtra 	  = surplusSize / rubberBands;
		    distributableSize -= rubberBandExtra * rubberBands;
		}
	    }
	}

	if ( debugLayout() )
	{
	    yuiDebug() << "Distributing extra space"				<< endl;
	    yuiDebug() << "\tnew size: "		<< newSize		<< endl;
	    yuiDebug() << "\tdistributable size: " 	<< distributableSize	<< endl;
	    yuiDebug() << "\trubber band extra: "	<< rubberBandExtra	<< endl;
	    yuiDebug() << "\trubber bands: "		<< rubberBands		<< endl;
	    yuiDebug() << "\ttotal weight: "		<< totalWeight		<< endl;
	    yuiDebug() << "\tnon weighted extra: "	<< nonWeightedExtra 	<< endl;
	}

	int i=0;
	for ( YWidgetListConstIterator it = childrenBegin();
	      it != childrenEnd();
	      ++it, i++ )
	{
	    YWidget * child = *it;

	    if ( child->hasWeight( primary() ) )
	    {
		// Weighted children will get their share.

		childSize[i] = distributableSize * child->weight( primary() ) / totalWeight;

		if ( childSize[i] < child->preferredSize( primary() ) )
		{
		    yuiDebug() << "Layout running out of space: "
			       << "Resizing child widget #" 		<< i << " ("<< child
			       << ") below its preferred size of "	<< child->preferredSize( primary() )
			       << " to " 				<< childSize[i]
			       << endl;
		}
	    }
	    else
	    {
		// Non-weighted children will get their preferred size.

		childSize[i] = child->preferredSize( primary() );


		if ( child->stretchable( primary() ) )
		{
		    // If there are only non-weighted children (and only then),
		    // the stretchable children will get their fair share of the
		    // extra space.

		    childSize[i] += nonWeightedExtra;
		}

		if ( isLayoutStretch( child, primary() ) )
		{
		    // If there is more than the total preferred size and there
		    // are rubber bands, distribute surplus space among the
		    // rubber bands.

		    childSize[i] += rubberBandExtra;
		}
	    }

	    childPos[i] = pos;
	    pos += childSize[i];
	}
    }
    else	// The pathological case: Not enough space.
    {
	/*
	 * We're in deep shit.
	 *
	 * Not only is there nothing to distribute among the weighted children,
	 * we also need to resize the non-weighted children below their preferred
	 * sizes. Let's at least treat them equally bad - divide the lost space
	 * among them as fair as possible.
	 */

	int 	tooSmall	 	= -distributableSize;
	int  	loserCount 	 	= 0;
	int 	totalMargins	 	= 0L;
	int 	remainingMargins	= 0L;
	double	marginScale		= 0.0;

	yuiDebug() << "Not enough space: " << tooSmall << " too small - check the layout!" << endl;


	// Maybe some of the children are YAlignments with margins that can be reduced

	for ( YWidgetListConstIterator it = childrenBegin();
	      it != childrenEnd();
	      ++it )
	{
	    if ( ! (*it)->hasWeight( primary() ) ) // children with weights will get nothing anyway
	    {
		YAlignment * alignment = dynamic_cast<YAlignment *> (*it);

		if ( alignment )
		{
		    totalMargins += alignment->totalMargins( primary() );
		    yuiDebug() << "Found alignment with margins" << endl;
		}
	    }
	}


	if ( totalMargins > tooSmall )	// We can make up for insufficient space just by reducing margins
	{
	    remainingMargins = totalMargins - tooSmall;
	    tooSmall = 0L;
	    marginScale = ( (double) remainingMargins ) / totalMargins;

	    yuiDebug() << "Making up for insufficient space by reducing margins to "
		       << 100.0 * marginScale << "% - "
		       << remainingMargins << " left for margins"
		       << endl;
	}
	else				// Reducing all margins to zero still doesn't solve the problem
	{
	    tooSmall -= totalMargins;

	    yuiDebug() << "Reducing all margins to 0, but still " << tooSmall << " too small" << endl;
	}


	// Calculate initial sizes

	int i=0;
	for ( YWidgetListConstIterator it = childrenBegin();
	      it != childrenEnd();
	      ++it, i++ )
	{
	    if ( ! (*it)->hasWeight( primary() ) )
	    {
		loserCount++;
		childSize[i] = (*it)->preferredSize( primary() );

		YAlignment * alignment = dynamic_cast<YAlignment *> (*it);

		if ( alignment )		// Alignment widgets may have margins we can reduce
		{
		    int margins = alignment->totalMargins( primary() );
		    childSize[i] -= margins;			// Strip off original margin

		    if ( remainingMargins > 0 )			// Anything left to redistribute?
		    {
			margins		 = (int) marginScale * margins;	// Scale down margin
			childSize[i]	 += margins;		// Add the scaled-down margin
			remainingMargins -= margins;		// Deduct from redistributable margin
		    }
		}
	    }
	    else
	    {
		// Weighted children will get nothing anyway if there is nothing
		// to distribute.

		childSize[i] = 0L;
	    }
	}


	// Distribute loss

	int oldTooSmall = tooSmall;
	int oldLoserCount = loserCount;
	while ( tooSmall > 0 && loserCount > 0 )
	{
	    if ( debugLayout() )
	    {
		yuiWarning() << "Distributing insufficient space of " << tooSmall
			     << " among " << loserCount << " losers"
			     << endl;
	    }

	    int dividedLoss = std::max( tooSmall / loserCount, 1 );

	    int i=0;
	    for ( YWidgetListConstIterator it = childrenBegin();
		  it != childrenEnd() && tooSmall > 0;
		  ++it, i++ )
	    {
		if ( childSize[i] < dividedLoss )
		{
		    // This widget is too small to take its share of the
		    // loss. We'll have to re-distribute the rest of the
		    // loss among the others. Arrgh.

		    if ( childSize[i] > 0L )
		    {
			tooSmall	-= childSize[i];
			childSize[i]	 = 0L;
			loserCount--;

			if ( loserCount > 0 )
			    dividedLoss = std::max( tooSmall / loserCount, 1 );
		    }
		}
		else
		{
		    childSize[i]	-= dividedLoss;
		    tooSmall		-= dividedLoss;
		}

		if ( debugLayout() )
		{
		    YWidget * child = *it;

		    yuiWarning() << "child #" << i <<" ( " << child
				 << " ) will get " 	<< childSize[i]
				 << " - "  		<< child->preferredSize( primary() ) - childSize[i] << " too small"
				 << " (preferred size: "<< child->preferredSize( primary() )
				 << ", weight: " 	<< child->weight( primary() )
				 << ", stretchable: " 	<< std::boolalpha << child->stretchable( primary() )
				 << "), pos: " 		<< childPos[i]
				 << endl;
		}
	    }

	    if ( oldTooSmall == tooSmall &&
                 oldLoserCount == loserCount )
	    {
		yuiWarning() << "Preventing endless loop while layout space distribution. Break." << endl;
		break;
	    }

	    oldTooSmall = tooSmall;
	    oldLoserCount = loserCount;
	}


	// Calculate postitions

	for ( int i = 0, pos=0; i < childrenCount(); i++ )
	{
	    childPos[i] = pos;
	    pos	     += childSize[i];

	}

    }
}


void
YLayoutBox::calcSecondaryGeometry( int	 	newSize,
				   sizeVector & childSize,
				   posVector  & childPos )
{
    int i=0;
    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it, i++ )
    {
	YWidget * child = *it;
	int preferred = child->preferredSize( secondary() );

	if ( child->stretchable( secondary() ) || newSize < preferred || preferred == 0 )
	    // Also checking for preferred == 0 to make HSpacing / VSpacing visible in YDialogSpy:
	    // Otherwise they would be 0 pixels wide or high, i.e. invisible
	{
	    childSize[i] = newSize;
	    childPos [i] = 0L;
	}
	else // child is not stretchable and there is more space than it wants
	{
	    childSize[i] = preferred;
	    childPos [i] = ( newSize - preferred ) / 2;	// center
	}

	if ( childSize[i] < preferred )
	{
	    yuiDebug() << "Layout running out of space: "
		       << "Resizing child widget #"		<< i
		       << " (" 					<< child
		       << ") below its preferred size of " 	<< preferred
		       << " to " 				<< childSize[i]
		       << endl;
	}

	if ( debugLayout() )
	{
	    ( childSize[i] < preferred ? yuiWarning() : yuiDebug() )
		<< "child #"		<< i
		<< " (" 		<< child
		<< ") will get " 	<< childSize[i]
		<< " (preferred size: " << preferred
		<< ", weight: " 	<< child->weight( secondary() )
		<< ", stretchable: " 	<< std::boolalpha << child->stretchable( secondary() )
		<< "), pos: " 		<< childPos[i]
		<< endl;
	}
    }
}


void
YLayoutBox::doResize( sizeVector & width,
		      sizeVector & height,
		      posVector  & x_pos,
		      posVector  & y_pos  )
{
    int i=0;
    for ( YWidgetListConstIterator it = childrenBegin();
	  it != childrenEnd();
	  ++it, i++ )
    {
	YWidget * child = *it;

	child->setSize( width[i], height[i] );
	moveChild( child, x_pos[i], y_pos[i] );

	if ( debugLayout() )
	{
	    yuiMilestone() << "  x: " << std::setw( 3 ) << x_pos[i]
			   << "  y: " << std::setw( 3 ) << y_pos[i]
			   << "  w: " << std::setw( 3 ) << width[i]
			   << "  h: " << std::setw( 3 ) << height[i]
			   << "  "   << child
			   << endl;
	}
    }
}


const char *
YLayoutBox::widgetClass() const
{
    return primary() == YD_VERT ? "YVBox" : "YHBox";
}
