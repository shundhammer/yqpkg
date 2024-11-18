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

  File:		YGraph.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#ifndef YGraph_h
#define YGraph_h

#include "YWidget.h"

/*
 * Do not include graphviz/types.h here since it conflicts with ncurses
 * headers. People should finally start to use C++ and namespaces!
 *
 * The previous workaround of inserting the graph_t definition here does not
 * work with graphviz >= 2.30.0 since it depends on the define WITH_CGRAPH.
 *
 * For that reason a lot of functions simply take a void* instead of graph_t*.
 */

class YGraphPrivate;

/**
 * A graph with nodes and edges, rendered with Graphviz.
 **/
class YGraph : public YWidget
{
protected:

    /**
     * Constructor.
     *
     * Loads a graph in DOT format from filename and uses the layout algorithm
     * layoutAlgorithm to layout and then render the graph. The layout
     * algorithm can be any string accepted by the function gvLayout from
     * graphviz, e.g. "dot" or "neato".
     **/
    YGraph( YWidget * parent, const std::string & filename, const std::string & layoutAlgorithm );

    /**
     * Constructor.
     *
     * Renders the graph. The graph must already contain layout information.
     **/
    YGraph( YWidget * parent, /* graph_t */ void * graph );

public:

    /**
     * Destructor.
     **/
    virtual ~YGraph();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YGraph"; }

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This function may throw YUIPropertyExceptions.
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string    & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw YUIPropertyExceptions.
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();

    /**
     * Return the filename that describes the graph.
     **/
    std::string filename() const;

    /**
     * Set the filename that describes the graph and render the graph.
     * Derived classes can reimplent this, but they should call this base
     * class method in the new implementation. Most derived classes only need
     * to implement renderGraph().
     **/
    virtual void setFilename( const std::string & filename );

    /**
     * Return the layout-algorithm used for the graph.
     **/
    std::string layoutAlgorithm() const;

    /**
     * Set the layout-algorithm used for the graph.  Derived classes can
     * reimplent this, but they should call this base class method in the new
     * implementation.
     **/
    virtual void setLayoutAlgorithm( const std::string & filename );

    /**
     * Render the graph. Derived classes can reimplent this, but they should
     * call this base class method in the new implementation. Most derived
     * classes only need to implement renderGraph().
     **/
    virtual void setGraph( /* graph_t */ void * graph );

    /**
     * Return name of activated node. Activation can happen due to e.g. single
     * right mouse click (context menu) or double left mouse click.
     */
    virtual std::string activatedNode() const;

protected:

    /**
     * Render the graph from the filename. Derived classes are required to
     * implement this.
     **/
    virtual void renderGraph( const std::string & filename, const std::string & layoutAlgorithm ) = 0;

    /**
     * Render the graph. Derived classes are required to implement this.
     **/
    virtual void renderGraph( /* graph_t */ void * graph ) = 0;

private:

    ImplPtr<YGraphPrivate> priv;

};


#endif // YGraph_h
