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

  File:		YQTimezoneSelector.cc

  Author:	Stephan Kulow <coolo@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <math.h>

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTimezoneSelector.h"
#include "YQWidgetCaption.h"
#include <yui/YEvent.h>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QToolTip>
#include <QIcon>
#include <QtGlobal>


class YQTimezoneSelectorPrivate
{
    QWidget *parent;

public:

    YQTimezoneSelectorPrivate( QWidget *p ) {
        parent = p;
        blink = 0;
        highlight = 0;
    }
    QImage _pix;
    QPoint _zoom;

    struct Location
    {
        QString country;
        double latitude;
        double longitude;
        QString zone;
        QString comment;
	QString tip;

        QPoint pix_pos;

        bool operator<(const Location& l2) const;
    };

    Location _best;

    QList<Location> locations;

    Location findBest( const QPoint &pos ) const;

    QTimer *blink;

    int highlight;

    QPoint pixPosition( const Location &pos ) const;

    QPoint pixToWindow( const QPoint &pos ) const;

    QPixmap cachePix;
};


static float
convert_pos (const QString &pos, int digits)
{
    if (pos.length() < 4 || digits > 9) return 0.0;

    QString whole = pos.left( digits + 1 );
    QString fraction = pos.mid( digits + 1 );

    float t1 = whole.toFloat();
    float t2 = fraction.toFloat();

    if (t1 >= 0.0)
        return t1 + t2/pow (10.0, fraction.length() );
    else
        return t1 - t2/pow (10.0, fraction.length());
}


bool YQTimezoneSelectorPrivate::Location::operator<(const Location& l1 ) const
{
    return l1.latitude < latitude;
}


YQTimezoneSelector::YQTimezoneSelector( YWidget * parent,
                                        const string & pixmap,
                                        const std::map<string,string> & timezones )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTimezoneSelector( parent, pixmap, timezones )
{
    d = new YQTimezoneSelectorPrivate( this );

    setWidgetRep( this );
    setMouseTracking(true);
    d->_pix.load( fromUTF8( pixmap ) );

    setStretchable( YD_HORIZ, true );
    setStretchable( YD_VERT,  true );

    char buf[4096];
    FILE *tzfile = fopen ("/usr/share/zoneinfo/zone.tab", "r");
    while (fgets (buf, sizeof(buf), tzfile))
    {
        if (*buf == '#') continue;

        QString sbuf = buf;
        QStringList arr = sbuf.trimmed().split( '\t' );

        int split_index = 1;
        while ( split_index < arr[1].length() && arr[1][split_index] != '-' && arr[1][split_index] != '+' )
            split_index++;

        YQTimezoneSelectorPrivate::Location loc;
        loc.country = arr[0];
        loc.zone = arr[2];
        std::map<string, string>::const_iterator tooltip = timezones.find( loc.zone.toStdString() );
        if (tooltip  == timezones.end() )
            continue;

        loc.tip = fromUTF8( tooltip->second );
        if ( arr.size() > 3 )
            loc.comment = arr[3];
        loc.latitude  = convert_pos ( arr[1].left( split_index ), 2);
        loc.longitude = convert_pos ( arr[1].mid( split_index ), 3);

        loc.pix_pos = d->pixPosition( loc );

        d->locations.push_back( loc );
    }

    fclose (tzfile);

    std::sort( d->locations.begin(), d->locations.end() );

    d->blink = new QTimer( this );
    d->blink->setInterval( 200 );
    connect( d->blink, &pclass(d->blink)::timeout,
             this,     &pclass(this)::slotBlink );

    d->highlight = 0;
}

YQTimezoneSelector::~YQTimezoneSelector()
{
    delete d;
    // NOP
}


int YQTimezoneSelector::preferredWidth()
{
    return 600;
}


int YQTimezoneSelector::preferredHeight()
{
    return 300;
}


void YQTimezoneSelector::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


QPoint YQTimezoneSelectorPrivate::pixPosition( const Location &pos ) const
{
    return QPoint(  (int) ( _pix.width()  / 2 + _pix.width()  / 2 * pos.longitude / 180 ),
                    (int) ( _pix.height() / 2 - _pix.height() / 2 * pos.latitude  /  90 ) ) ;
}


void YQTimezoneSelector::mousePressEvent ( QMouseEvent * event )
{
    if ( event->button() == Qt::RightButton )
    {
        d->_zoom = QPoint();
        d->cachePix = QPixmap();
    }
    else if ( event->button() == Qt::LeftButton )
    {
        d->_best = d->findBest( event->pos() );

        if ( d->_zoom.isNull() )
        {
            QPoint click = event->pos();
            /* keep the zoom point in unscaled math */
            d->_zoom.rx() = (int) ( double( click.x() ) * d->_pix.width()   / width() );
            d->_zoom.ry() = (int) ( double( click.y() ) * d->_pix.height() / height() );
        }

        d->cachePix = QPixmap();

        if ( notify() )
            YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );

        d->blink->start();
    } else
        return;

    update();
}


void YQTimezoneSelector::paintEvent( QPaintEvent *event )
{
    QFrame::paintEvent( event );
    QPainter p( this );

    if ( d->cachePix.width() != width() || d->cachePix.height() != height() )
        d->cachePix = QPixmap();

    if ( d->_zoom.isNull() )
    {
        if ( d->cachePix.isNull() )
        {
            QImage t = d->_pix.scaled( width(),  height(), Qt::KeepAspectRatio );
            d->cachePix = QPixmap::fromImage( t );
        }
        p.drawPixmap( ( width() - d->cachePix.width() ) / 2, ( height() - d->cachePix.height() ) / 2, d->cachePix );

        Q_INIT_RESOURCE( qt_icons );
        QIcon icon = QIcon::fromTheme( "zoom-in", QIcon( ":/zoom-in" ) );

        if ( !icon.isNull() )
            setCursor( QCursor( icon.pixmap( QSize( 16, 16 ) ) ) );
    }
    else
    {
        int left = qMin( qMax( d->_zoom.x() - width()  / 2, 0 ), d->_pix.width()  - width() );
        int top  = qMin( qMax( d->_zoom.y() - height() / 2, 0 ), d->_pix.height() - height() );

        if ( d->cachePix.isNull() )
            d->cachePix = QPixmap::fromImage( d->_pix.copy( QRect( QPoint( left, top ), size() ) ) );

        p.drawPixmap( 0, 0, d->cachePix );

        setCursor( Qt::CrossCursor );
    }

    p.setBrush( QColor( "#D8DF57" ) );
    p.setPen( QColor( "#B9DFD6" ) );

    for ( QList<YQTimezoneSelectorPrivate::Location>::const_iterator it = d->locations.begin(); it != d->locations.end(); ++it )
    {
        if ( !d->highlight || ( *it ).zone != d->_best.zone )
        {
            if ( d->_zoom.isNull() )
                p.drawEllipse( QRect( d->pixToWindow( ( *it ).pix_pos ) - QPoint( 1,1 ), QSize( 3, 3 ) ) );
            else
                p.drawEllipse( QRect( d->pixToWindow( ( *it ).pix_pos ) - QPoint( 2,2 ), QSize( 5, 5 ) ) );
        }
    }
    if ( d->highlight > 0 )
    {
        static const QColor blinks[] = { QColor( "#00ff00" ), QColor( "#22dd00" ), QColor( "#44bb00" ),
                                         QColor( "#669900" ), QColor( "#887700" ), QColor( "#aa5500" ),
                                         QColor( "#887700" ), QColor( "#669900" ), QColor( "#44bb00" ),
                                         QColor( "#22dd00" ) };
        int index = d->highlight - 1;
        p.setPen( blinks[ index ] );
        p.setBrush( blinks[ index ] );

        p.drawEllipse( QRect( d->pixToWindow( d->_best.pix_pos ) - QPoint( 2,2 ), QSize( 5, 5 ) ) );

        QFont f( font() );
        f.setBold( true );
        p.setFont( f );
        QFontMetrics fm( f );

        QPoint off = d->pixToWindow( d->_best.pix_pos ) + QPoint( 11, 4 );
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        int tw = fm.horizontalAdvance( d->_best.tip );
#else
        // Leap 15.0 has an older Qt version, make sure it also compiles there
        int tw = fm.width( d->_best.tip );
#endif
        if ( tw + off.x() > width() )
            off.rx() = d->pixToWindow( d->_best.pix_pos ).x() - tw - 10;

        p.setPen( Qt::black );
        p.drawText( off, d->_best.tip );

        p.setPen( Qt::white );
        p.drawText( off - QPoint( 1, 1 ), d->_best.tip );

    }
}


YQTimezoneSelectorPrivate::Location YQTimezoneSelectorPrivate::findBest( const QPoint &pos ) const
{
    double min_dist = 2000;
    Location best;
    for ( QList<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it )
    {
        double dist = QPoint( pixToWindow( ( *it ).pix_pos ) - pos ).manhattanLength ();
        if ( dist < min_dist )
        {
            min_dist = dist;
            best = *it;
        }
    }
    return best;
}


bool YQTimezoneSelector::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        YQTimezoneSelectorPrivate::Location best = d->findBest( helpEvent->pos() );
        QToolTip::showText(helpEvent->globalPos(), best.tip );
    }
    return QWidget::event(event);
}


string YQTimezoneSelector::currentZone() const
{
    return d->_best.zone.toStdString();
}


QPoint YQTimezoneSelectorPrivate::pixToWindow( const QPoint &pos ) const
{
    if ( _zoom.isNull() )
    {
        return QPoint( (int) ( double( pos.x() ) * cachePix.width()  / _pix.width()  ) + ( parent->width() - cachePix.width() ) / 2,
                       (int) ( double( pos.y() ) * cachePix.height() / _pix.height() ) + ( parent->height() - cachePix.height() ) /2 );
    }
    int left = qMin( qMax( _zoom.x() - parent->width() / 2, 0 ), _pix.width() - parent->width() );
    int top  = qMin( qMax( _zoom.y() - parent->height() / 2, 0 ), _pix.height() - parent->height() );

    return QPoint( pos.x() - left, pos.y() - top );
}


void YQTimezoneSelector::setCurrentZone( const string &_zone, bool zoom )
{
    QString zone = fromUTF8( _zone );

    if ( d->_best.zone == zone )
        return;

    d->_best = YQTimezoneSelectorPrivate::Location();

    for ( QList<YQTimezoneSelectorPrivate::Location>::const_iterator it = d->locations.begin(); it != d->locations.end(); ++it )
    {
        if ( ( *it ).zone == zone )
            d->_best = *it;
    }

    if ( zoom )
        d->_zoom = d->_best.pix_pos;
    else
        d->_zoom = QPoint();

    d->cachePix = QPixmap();
    d->highlight = 1;

    d->blink->start();
    update();
}


void YQTimezoneSelector::slotBlink()
{
    if ( d->_best.zone.isNull() )
    {
        d->blink->stop();
        return;
    }

    if ( d->highlight++ > 9 )
        d->highlight = 1;

    QPoint current = d->pixToWindow( d->_best.pix_pos );
    update( QRect( current - QPoint( 3, 3 ), QSize( 7, 7 ) ) );
}


