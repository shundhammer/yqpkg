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

  File:	      YQMultiProgressMeter.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include <qevent.h>
#include <QPointF>
#include <QStyleOptionProgressBar>
#include <QDebug>
#include "YQUI.h"
#include "YQMultiProgressMeter.h"
#include <yui/YDialog.h>

using std::endl;



YQMultiProgressMeter::YQMultiProgressMeter( YWidget *			parent,
					    YUIDimension		dim,
					    const vector<float> &	maxValues )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YMultiProgressMeter( parent, dim, maxValues )
{
    init();
    setWidgetRep( this );
}


YQMultiProgressMeter::~YQMultiProgressMeter()
{
    // NOP
}


void YQMultiProgressMeter::init()
{
    _margin 		= 2;
    _segmentMinLength 	= 12;
    _triSpacing		= 1;
    _spacing	= 2;
    setTriThickness( 4 );
}


void YQMultiProgressMeter::doUpdate()
{
    QWidget::update();
}


void YQMultiProgressMeter::paintEvent ( QPaintEvent * event )
{
    if ( ! event )
	return;

    QPainter painter( this );

//     if ( ! event->erased() )
// 	   painter.eraseRect( event->rect() );

    int totalLength 	= horizontal() ? width() : height();
    int thickness 	= horizontal() ? height() : width();

    totalLength 	-= 2 * margin() + spacing() * ( segments()-1 );
    thickness		-= 2 * margin();

    if ( triThickness() > 0 )
	thickness -= 2 * triThickness() + 2 * triSpacing();

    if ( totalLength < 1 || thickness < 1 || segments() < 1 )
	return;


    // Add up the total sum of all maxValues

    float totalSum = 0.0;

    for ( int i=0; i < segments(); i++ )
	totalSum += maxValue( i );


    // Figure out minimal segment length

    int minLength = segmentMinLength();


    // Limit the minimum if there isn't even that much space

    if ( minLength * segments() > totalLength )
	minLength = totalLength / ( 2 * segments() );


    // First attempt of scaling factor from values to pixel coordinates

    if ( totalSum == 0.0 )
    {
	yuiError() << "Avoiding division by zero: totalSum" << endl;
	return;
    }

    float scale = ( (float) totalLength ) / totalSum;
    float scaledMinLength = ( (float) minLength ) / scale;


    // Check how many segments would become smaller than the minimum

    int smallSegmentsCount = 0;
    float restSum = 0.0;

    for ( int i=0; i < segments(); i++ )
    {
	if ( maxValue( i ) < scaledMinLength )
	    smallSegmentsCount++;
	else
	    restSum += maxValue( i );
    }


    // Small segments that get at least minLength pixels consume more screen
    // space than initially planned, so recompute what is left for the others.

    int distributableLength = totalLength - smallSegmentsCount * minLength;

    if ( restSum == 0.0 )
    {
	yuiError() << "Avoiding division by zero: restSum" << endl;
	return;
    }

    // Recompute scale to take small segments into account that now get screen
    // space disproportional to their real size (maxValue).
    scale = ( (float) distributableLength ) / ( restSum );

    // Set up painter

    if ( vertical() )
    {
	painter.rotate( 90 );
	painter.scale( 1.0, -1.0 );
    }

    int offset = margin();

    // Draw each segment in turn

    for ( int i=0; i < segments(); i++ )
    {
	int length;

	if ( maxValue( i ) < scaledMinLength )
	    length = minLength;
	else
	    length = (int) ( maxValue( i ) * scale + 0.5 );

	drawSegment( i, painter, offset, length, thickness );

	if ( i > 0 )
	    drawMarkers( painter, offset, thickness );

	offset += length + spacing();
    }
}


void YQMultiProgressMeter::drawSegment( int segment,
					QPainter & painter,
					int offset,
					int length,
					int thickness )
{
    //
    // Fill segment
    //
    // Vertical MultiProgressMeters will be filled thermometer-like from bottom
    // to top, horizontal ones like normal progress bars from left to right,
    // i.e. just the opposite way.
    //

    int border = margin();

    if ( triThickness() > 0 )
        border += triThickness() + triSpacing();

    if ( maxValue( segment ) == 0.0 )
    {
        yuiError() << "Avoiding division by zero: maxValue[" << segment << "]" << endl;
        return;
    }

    // Use 0..1000 range to avoid overflow with huge numbers (Gigabytes).
    const int scaledMax = 1000;
    int scaledProgress =
	(int) ( 0.5 + ( currentValue( segment ) / maxValue( segment ) ) * ( (float) scaledMax ) );

    if ( vertical() )	// fill thermometer-like from bottom to top
    {
        QStyleOptionProgressBar opts;
        opts.initFrom(this);
        opts.progress = scaledMax - scaledProgress;
        opts.minimum = 0;
        opts.maximum = scaledMax;
        opts.invertedAppearance = true;
        opts.rect = QRect( offset, border, length, thickness );
        style()->drawControl(QStyle::CE_ProgressBarGroove, &opts, &painter, this);

	if ( opts.progress > 0 )
	    style()->drawControl(QStyle::CE_ProgressBarContents, &opts, &painter, this);
    }
    else	// horizontal - fill from left to right like a normal progress bar
    {
        QStyleOptionProgressBar opts;
        opts.initFrom(this);
        opts.progress = scaledProgress;
        opts.minimum = 0;
        opts.maximum = scaledMax;
        opts.rect = QRect( offset, border, length, thickness );

        style()->drawControl(QStyle::CE_ProgressBarGroove, &opts, &painter, this);
	if ( opts.progress > 0 )
            style()->drawControl(QStyle::CE_ProgressBarContents, &opts, &painter, this);
    }
}


void YQMultiProgressMeter::drawMarkers( QPainter & painter, int offset, int thickness )
{
    if ( triThickness() < 1 )
	return;

    offset -= spacing() / 2 + 1; 	// integer division rounds down!

    const QBrush & color = palette().windowText();
    painter.setBrush( color );
    // painter.setBrush( NoBrush );


    // Draw upper marker triangle

    int tri = triThickness();

    QPointF points[3] =
	{
	    QPointF( offset - tri+1,	margin() ),		// top left (base)
	    QPointF( offset,		margin() + tri-1 ),	// lower center (point)
	    QPointF( offset + tri-1, 	margin() )		// top right (base)
	};

    painter.drawConvexPolygon( points, 3 );

    // Draw lower marker triangle

    int pointOffset = margin() + tri + thickness + 2 * triSpacing();

    QPointF points2[3] =
	{
	    QPointF( offset,		pointOffset ),		// top center (point)
	    QPointF( offset + tri-1,	pointOffset + tri-1 ),	// top right (base)
	    QPointF( offset - tri+1,	pointOffset + tri-1 )	// bottom left (base)
	};

    painter.drawConvexPolygon( points2, 3 );
}


int YQMultiProgressMeter::thickness()
{
    int thickness = 23;
    thickness += 2 * margin();

    if ( triThickness() > 0 )
	thickness += 2 * triThickness() + 2 * triSpacing();

    return thickness;
}


int YQMultiProgressMeter::length()
{
    int length = 70 * segments() + 2 * margin();

    return length;
}


void YQMultiProgressMeter::setTriThickness( int value )
{
    _triThickness = value;

    if ( _triThickness < 1 )
	setTriSpacing( 0 );
}


void YQMultiProgressMeter::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    QWidget::update();
    YWidget::setEnabled( enabled );
}


int YQMultiProgressMeter::preferredWidth()
{
    return horizontal() ? length() : thickness();
}


int YQMultiProgressMeter::preferredHeight()
{
    return horizontal() ? thickness() : length();
}


void YQMultiProgressMeter::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    doUpdate();
}
