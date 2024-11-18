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

  File:	      YQBarGraph.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <algorithm>
#include <QPainter>
#include <QToolTip>
#include <QHelpEvent>

#include "utf8.h"
#include "YQUI.h"
#include "YQBarGraph.h"


#define YQBarGraphOuterMargin		YQWidgetMargin
#define YQBarGraphLabelHorizontalMargin	1
#define YQBarGraphLabelVerticalMargin	2
#define YQBarGraphMinWidth		80
#define YQBarGraphMinHeight		30



YQBarGraph::YQBarGraph( YWidget * parent )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YBarGraph( parent )
{
    setWidgetRep( this );
}


YQBarGraph::~YQBarGraph()
{
    // NOP
}


void
YQBarGraph::doUpdate()
{
    QFrame::update(); // triggers drawContents()
}


QString
YQBarGraph::segmentText( unsigned index ) const
{
    const YBarGraphSegment & seg = segment( index );
    QString text = fromUTF8( seg.label() );

    if ( text.contains( "%1" ) )
        text = text.arg( seg.value() );   // substitute variable

    return text;
}


int
YQBarGraph::findSegment( int xPos ) const
{
    for ( int i = segments() - 1; i >= 0; i-- )
    {
        if ( i < _segStart.size() )
        {
            if ( xPos >= _segStart.value( i ) )
                return i;
        }
    }

    return -1;
}


bool
YQBarGraph::event( QEvent *event )
{
    if  ( event->type() == QEvent::ToolTip )
    {
	QHelpEvent * helpEvent = static_cast<QHelpEvent *>( event );
        int index = findSegment( helpEvent->x() );

        if ( index >= 0 )
            QToolTip::showText( helpEvent->globalPos(), segmentText( (unsigned) index ) );
    }

     return QWidget::event( event );
}


void
YQBarGraph::paintEvent( QPaintEvent* paintEvent )
{
    QFrame::paintEvent( paintEvent );

    QPainter painter( this );

    unsigned nextDefaultColor = 0;
    int totalWidth 	= contentsRect().width()  - 2*YQBarGraphOuterMargin;
    int segHeight  	= contentsRect().height() - 2*YQBarGraphOuterMargin;
    int x_off		= YQBarGraphOuterMargin;
    int y_off 		= YQBarGraphOuterMargin;
    int valueTotal 	= 0;
    QFontMetrics fm	= painter.fontMetrics();

    _segStart.clear();

    for ( int i=0; i < segments(); i++ )
	valueTotal += segment(i).value();

    if ( valueTotal == 0 ) // Avoid division by zero
	return;

    for ( int i=0; i < segments(); i++ )
    {
	const YBarGraphSegment & seg = segment(i);
	int segWidth = ( (long) totalWidth * seg.value() ) / valueTotal;
	int stringWidth = 0;

	if ( i == segments()-1 )
	{
	    // Compensate for rounding errors:
	    // The last segment gets all leftover pixels from the previous ones.

	    segWidth = totalWidth - x_off + YQBarGraphOuterMargin;
	}


	//
	// Fill the segment
	//

	YColor segmentColor = seg.segmentColor();
	YColor textColor    = seg.textColor();

	if ( segmentColor.isUndefined() || textColor.isUndefined() )
	{
	    // If any of the colors is undefined, use the next default color
	    // for both so some contrast is ensured.

	    segmentColor = defaultSegmentColor( nextDefaultColor   );
	    textColor	 = defaultTextColor   ( nextDefaultColor++ );
	}

	painter.setBrush( QColor( segmentColor.red(),
				   segmentColor.green(),
				   segmentColor.blue() ) );
	painter.setPen( Qt::NoPen );
	painter.drawRect( x_off, y_off, segWidth+2, segHeight+2 );


	//
	// Draw the label
	//

	painter.setPen( Qt::SolidLine );
	painter.setPen( QColor( textColor.red(),
                                textColor.green(),
                                textColor.blue() ) );

        QString txt = segmentText( i );
	stringWidth = fm.size( 0, txt ).width();

	// draw the text only if it fits the current segment width ...
	if ( stringWidth < segWidth )
	{
	    painter.drawText( x_off + YQBarGraphLabelHorizontalMargin,
                              y_off + YQBarGraphLabelVerticalMargin,
                              segWidth  - 2 * YQBarGraphLabelHorizontalMargin + 1,
                              segHeight - 2 * YQBarGraphLabelVerticalMargin   + 1,
                              Qt::AlignCenter, txt );
	}

	// store the segment starting position for the tooltips
        _segStart << x_off;

	// Prepare for the next segment

	x_off += segWidth;
    }
}


YColor
YQBarGraph::defaultSegmentColor( unsigned index )
{
    // use color from qproperty defined in qss
    QColor color;
    QStringList colors = _backgroundColors.split(",");

    if ( colors.size() > 0 )
    {
        color = colors[index % colors.size()];
        if ( color.isValid() )
            return YColor( color.red(), color.green(), color.blue() );
    }

    // use default color if color is not defined in qss
    switch( index % 8 )
    {
	case 0:	return YColor(   0,   0, 128 );	// dark blue
	case 1:	return YColor(  64, 200, 255 );	// medium blue
	case 2:	return YColor( 255, 255, 255 ); // white
	case 3:	return YColor(   0, 153, 153 );	// cadet blue
	case 4:	return YColor( 150, 255, 255 ); // cyan
	case 5:	return YColor( 100, 100, 100 ); // medium grey
	case 6:	return YColor(   0, 200, 100 ); // medium green
	case 7:	return YColor(   0, 100,  76 ); // dark green
    }

    return YColor( 255, 255, 255 ); // just to make gcc happy
}


YColor
YQBarGraph::defaultTextColor( unsigned index )
{
    // use color from qproperty defined in qss

    QColor color;
    QStringList colors = _foregroundColors.split(",");

    if ( colors.size() > 0 )
    {
        color = colors[index % colors.size()];
        if (color.isValid() )
            return YColor( color.red(), color.green(), color.blue() );
    }

    // use default color if color is not defined in qss
    YColor black = YColor(   0,   0,   0 );
    YColor white = YColor( 255, 255, 255 );

    switch( index % 8 )
    {
	case 0:	return white;
	case 1:	return black;
	case 2:	return black;
	case 3:	return black;
	case 4:	return black;
	case 5:	return white;
	case 6:	return black;
	case 7:	return white;
    }

    return black; // just to make gcc happy
}


void
YQBarGraph::setEnabled( bool enabled )
{
    QFrame::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQBarGraph::preferredWidth()
{
    int width  = 0;
    QFontMetrics metrics = fontMetrics();

    for ( int i=0; i < segments(); i++ )
    {
	QString txt = fromUTF8( segment(i).label() );

	if ( txt.contains( "%1" ) )
	    txt = txt.arg( segment(i).value() );

	QSize segSize = metrics.size( 0, txt );
	width += segSize.width();
    }

    width += 2 * YQBarGraphLabelHorizontalMargin;
    width += frameWidth();
    width += 2 * YQBarGraphOuterMargin;
    width  = std::max( width, YQBarGraphMinWidth );

    return width;
}


int
YQBarGraph::preferredHeight()
{
    int height = YQBarGraphMinHeight;
    QFontMetrics metrics = fontMetrics();

    for ( int i=0; i < segments(); i++ )
    {
	QString txt = fromUTF8( segment(i).label() );

	if ( txt.contains( "%1" ) )
	    txt = txt.arg( segment(i).value() );

	QSize segSize = metrics.size( 0, txt );
	height = std::max( height, segSize.height() );
    }

    height += 2 * YQBarGraphLabelVerticalMargin;
    height += frameWidth();
    height += 2 * YQBarGraphOuterMargin;
    height  = std::max( height, YQBarGraphMinHeight );

    return height;
}


void
YQBarGraph::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


QString YQBarGraph::backgroundColors()
{
    return _backgroundColors;
}


void YQBarGraph::setBackgroundColors( QString colors )
{
    _backgroundColors = colors;
}


QString YQBarGraph::foregroundColors()
{
    return _foregroundColors;
}


void YQBarGraph::setForegroundColors( QString colors )
{
    _foregroundColors = colors;
}




