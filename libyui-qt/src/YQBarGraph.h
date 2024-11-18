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

  File:	      YQBarGraph.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQBarGraph_h
#define YQBarGraph_h

#include <QFrame>
#include <QEvent>
#include <QVector>
#include <yui/YBarGraph.h>

using namespace std;


class YQBarGraph : public QFrame, public YBarGraph
{
    Q_OBJECT
    Q_PROPERTY(QString BackgroundColors READ backgroundColors WRITE setBackgroundColors DESIGNABLE true)
    Q_PROPERTY(QString ForegroundColors READ foregroundColors WRITE setForegroundColors DESIGNABLE true)

public:

    /**
     * Constructor.
     **/
    YQBarGraph( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQBarGraph();

    /**
     * Perform a visual update on the screen.
     *
     * Implemented from YBarGraph.
     **/
    virtual void doUpdate();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

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
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );


protected:

    /**
     * General event handler. Reimplemented for dynamic tooltips.
     **/
    virtual bool event( QEvent * event );

    /**
     * Draw the contents.
     *
     * Reimplemented from QFrame.
     **/
    virtual void paintEvent( QPaintEvent * painter );

    /**
     * Return one from a set of default segment background colors.
     **/
    YColor defaultSegmentColor( unsigned index );

    /**
     * Return one from a set of default text colors. This text color is
     * guaranteed to contrast with the defaultSegmentColor with the same index.
     **/
    YColor defaultTextColor( unsigned index );

    /**
     * Return the text for a segment with a '%1' placeholder (if present)
     * expanded with that segment's numerical value.
     **/
    QString segmentText( unsigned index ) const;

    /**
     * Find a segment index by position. Return -1 if not found.
     *
     * Notice that this is only meaningful after the first paintEvent() where
     * those positions are initialized.
     **/
    int findSegment( int xPos ) const;


    // QSS doesn't allow to store a list of QColors, that's the reason
    // why we use QString and store the colors in the following format:
    //
    //   YQBarGraph
    //   {
    //       qproperty-BackgroundColors: "#aabbcc,#bbccdd,#eeff00"";
    //       qproperty-ForegroundColors: "black,yellow,white";
    //   }

    QString backgroundColors();
    void    setBackgroundColors( QString colors );

    QString foregroundColors();
    void    setForegroundColors( QString colors );


    //
    // Data members
    //


    QString _foregroundColors;
    QString _backgroundColors;

    QVector<int> _segStart; // x position of each segment
};


#endif // YQBarGraph_h
