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

  File:		YLayoutBox.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YLayoutBox_h
#define YLayoutBox_h

#include <vector>
#include "YWidget.h"


class YLayoutBoxPrivate;

/**
 * A vertical or horizontal stacking of widgets, implementing HBox and VBox.
 **/
class YLayoutBox : public YWidget
{
public:
    typedef std::vector<int> sizeVector;
    typedef std::vector<int> posVector;

protected:
    /**
     * Constructor.
     *
     * Creates a VBox for dim == YD_VERT or a HBox for YD_HORIZ.
     **/
    YLayoutBox( YWidget * parent, YUIDimension dim );

public:
    /**
     * Destructor.
     **/
    virtual ~YLayoutBox();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Return the primary dimension, i.e., the dimension this LayoutBox lays
     * out its children in: YD_VERT for a VBox, YD_HORIZ for a HBox.
     **/
    YUIDimension primary() const;

    /**
     * Return the secondary dimension.
     **/
    YUIDimension secondary() const;

    /**
     * Returns 'true' if layout debugging (verbose logging during layout) is on.
     **/
    bool debugLayout() const;

    /**
     * Enable or disable layout debugging.
     **/
    void setDebugLayout( bool deb = true );

    /**
     * Preferred size of the widget in the specified dimension.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredSize( YUIDimension dim );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Sets the size of the layout box. This is where the layout policy
     * is implemented.
     *
     * Derived classes can reimplement this, but this base class method should
     * be called in the reimplemented function.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Returns the stretchability of the layout box:
     * The layout box is stretchable if one of the children is stretchable in
     * this dimension or if one of the child widgets has a layout weight in
     * this dimension.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool stretchable( YUIDimension dimension ) const;

    /**
     * Move a child to a new position.
     *
     * Derived classes are required to implement this.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY ) = 0;

    /**
     * Check if this is a layout stretch widget in the specfied dimension,
     * i.e. an empty widget that is stretchable.
     **/
    static bool isLayoutStretch( YWidget * child, YUIDimension dimension );

protected:

    /**
     * Add up all the children's weights.
     **/
    int childrenTotalWeight( YUIDimension dimension );

    /**
     * Return the maximum preferred size of all children in the specified
     * dimension.
     **/
    int childrenMaxPreferredSize( YUIDimension dimension );

    /**
     * Add up all the non-weighted children's preferred sizes in the specified
     * dimension.
     **/
    int totalNonWeightedChildrenPreferredSize( YUIDimension dimension );

    /**
     * Count the number of non-weighted children.
     **/
    int countNonWeightedChildren( YUIDimension dimension );

    /**
     * Count the number of stretchable ( non-weighted ) children.
     * Note: Weighted children are _always_ considered stretchable.
     **/
    int countStretchableChildren( YUIDimension dimension );

    /**
     * Count the number of "rubber bands", i.e. the number of
     * stretchable layout spacings ( e.g. {H|V}Weight,
     * {H|V}Spacing ). Only those without a weight are counted.
     **/
    int countLayoutStretchChildren( YUIDimension dimension );

    /**
     * Determine the number of the "dominating child" - the child widget that
     * determines the overall size with respect to its weight.
     *
     * Return 0 if there is no dominating child, i.e. none of the children has
     * a weight specified.
     **/
    YWidget * findDominatingChild();

    /**
     * Calculate the sizes and positions of all children in the primary
     * dimension and store them in "childSize" and "childPos".
     **/
    void calcPrimaryGeometry	( int		newSize,
				  sizeVector &	childSize,
				  posVector  &	childPos );

    /**
     * Calculate the sizes and positions of all children in the secondary
     * dimension and store them in "childSize" and "childPos".
     **/
    void calcSecondaryGeometry	( int		newSize,
				  sizeVector &	childSize,
				  posVector  &	childPos );

    /**
     * Actually perform resizing and moving the child widgets to the
     * appropriate position.
     *
     * The vectors passed are the sizes previously calculated by
     * calcPrimaryGeometry() and calcSecondaryGeometry().
     **/
    void doResize( sizeVector & width,
		   sizeVector & height,
		   posVector  & x_pos,
		   posVector  & y_pos  );


private:

    ImplPtr<YLayoutBoxPrivate>	priv;
};


typedef YLayoutBox YVBox;
typedef YLayoutBox YHBox;


#endif // YLayoutBox_h
