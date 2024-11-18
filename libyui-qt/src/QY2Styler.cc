/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2022 SUSE LLC
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

  File:		QY2Styler.cc

  Author:	Stefan Kulow <coolo@suse.de>

/-*/


#define YUILogComponent "qt-styler"
#include <yui/YUILog.h>
#include <yui/YUIException.h>
#include <yui/YSettings.h>
#include <yui/YDialog.h>

#include "QY2Styler.h"
#include "YQRichText.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QPixmapCache>
#include <QRegularExpression>
#include <QWidget>

#include <iostream>

#define LOGGING_CAUSES_QT4_THREADING_PROBLEMS	1

std::ostream & operator<<( std::ostream & stream, const QString     & str     );
std::ostream & operator<<( std::ostream & stream, const QStringList & strList );
std::ostream & operator<<( std::ostream & stream, const QWidget     * widget  );

using namespace std;


QY2Styler::QY2Styler( QObject *       parent,
                      const QString & defaultStyleSheet,
                      const QString & alternateStyleSheet)
    : QObject( parent )
{
    QPixmapCache::setCacheLimit( 5 * 1024 );
    // yuiDebug() << "Styler created" << endl;

    setDefaultStyleSheet  ( defaultStyleSheet   );
    setAlternateStyleSheet( alternateStyleSheet );

    _currentStyleSheet = QString( "" );
}


QY2Styler *
QY2Styler::styler()
{
    static QY2Styler * styler = 0;

    if ( ! styler )
    {
        // yuiDebug() << "Creating QY2Styler singleton" << endl;

        QString y2style    = getenv("Y2STYLE");
        QString y2altstyle = getenv("Y2ALTSTYLE");
        QString y2alttheme = y2altstyle + ".qss";

        styler = new QY2Styler( qApp, y2style, y2alttheme );
        YUI_CHECK_NEW( styler );

        if ( y2altstyle.isEmpty() || ! styler->styleSheetExists( y2alttheme ) )
            styler->loadDefaultStyleSheet();
        else
            styler->loadAlternateStyleSheet();
    }

    return styler;
}


bool
QY2Styler::styleSheetExists( const QString & styleSheet )
{
    QFileInfo fileInfo( themeDir() + styleSheet );
    return fileInfo.isFile();
}


void
QY2Styler::setDefaultStyleSheet( const QString & styleSheet )
{
    if ( ! styleSheetExists( styleSheet ) )
        return;

    _defaultStyleSheet = styleSheet;
    yuiDebug() << "Setting high-contrast style sheet to "
               << _defaultStyleSheet << endl;
}


void
QY2Styler::setAlternateStyleSheet( const QString & styleSheet )
{
    if ( ! styleSheetExists( styleSheet ) )
        return;

    _alternateStyleSheet = styleSheet;
    yuiDebug() << "Setting default style sheet to "
               << _alternateStyleSheet << endl;
}


bool
QY2Styler::loadDefaultStyleSheet()
{
    if ( ! loadStyleSheet( _defaultStyleSheet ) )
        return false;

    _usingAlternateStyleSheet = false;
    return true;
}


bool
QY2Styler::loadAlternateStyleSheet()
{
    if ( ! loadStyleSheet( _alternateStyleSheet ) )
        return false;

    _usingAlternateStyleSheet = true;
    return true;
}


bool
QY2Styler::loadStyleSheet( const QString & filename )
{
    QFile file( themeDir() + filename );

    if ( file.open( QIODevice::ReadOnly ) )
    {
        yuiMilestone() << "Using style sheet \"" << file.fileName() << "\"" << endl;
        QString text = file.readAll();
        _currentStyleSheet = QString( filename );
        setStyleSheet( text );
        return true;
    }
    else
    {
        yuiMilestone() << "Couldn't open style sheet \"" << file.fileName() << "\"" << endl;
        return false;
    }
}


QString
QY2Styler::buildStyleSheet( const QString & content )
{
    QStringList alreadyImportedFilenames;

    return buildStyleSheet( content, alreadyImportedFilenames );
}


QString
QY2Styler::buildStyleSheet( const QString & orig,
                            QStringList   & alreadyImportedFilenames_ret )
{
    QString result = orig;
    QRegularExpression re(" *@import +url\\(\"(.+)\"\\);");
    QRegularExpressionMatchIterator it = re.globalMatch( result );

    while ( it.hasNext() )
    {
        QRegularExpressionMatch match = it.next();
        QString fullPath = themeDir() + match.captured(1);
        result.replace( match.captured( 0 ),
                        buildStyleSheetFromFile( fullPath, alreadyImportedFilenames_ret ) );
    }

    return result;
}


QString
QY2Styler::buildStyleSheetFromFile( const QString & filename,
                                    QStringList   & alreadyImportedFilenames_ret )
{
    QFile file(filename);

    if ( ! alreadyImportedFilenames_ret.contains( filename ) && file.open( QIODevice::ReadOnly ) )
    {
        alreadyImportedFilenames_ret << filename;

        return buildStyleSheet( QString( file.readAll() ), alreadyImportedFilenames_ret );
    }
    else
        return "";
}


void
QY2Styler::setStyleSheet( const QString & text )
{
    _style = buildStyleSheet( text );
    processUrls( _style );

    QWidget *child;
    QList< QWidget* > childlist;

    foreach ( childlist, _children )
    {
        foreach ( child, childlist )
        {
            child->setStyleSheet( _style );
        }
    }

    foreach ( QWidget *registered_widget, _registered_widgets )
    {
        registered_widget->setStyleSheet( _style );
    }

    setRichTextStyleSheet( YDialog::topmostDialog( false ) );
}


void QY2Styler::setRichTextStyleSheet( YWidget * dialog )
{
    if ( ! dialog )
        return;

    QWidget * parent = (QWidget *) dialog->widgetRep();

    foreach ( YQRichText * richText, parent->findChildren<YQRichText *>() )
    {
        richText->setRichTextStyleSheet( _textStyle );
        // YQRichText does its own logging in that function,
        // no need to do it here as well
    }
}


void
QY2Styler::toggleAlternateStyleSheet()
{
    if ( usingAlternateStyleSheet() )
        loadDefaultStyleSheet();
    else
        loadAlternateStyleSheet();
}


void
QY2Styler::processUrls( QString & text )
{
    QString result;
    QStringList lines = text.split( '\n' );
    QRegExp urlRegex( ": *url\\((.*)\\)" );
    QRegExp backgroundRegex( "^ */\\* *Background: *([^ ]*) *([^ ]*) *\\*/$" );
    QRegExp richTextRegex( "^ */\\* *Richtext: *([^ ]*) *\\*/$" );

    _backgrounds.clear();

    for ( QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it )
    {
        QString line = *it;

	// Replace file name inside url() with full path (from themeDir() )

        if ( urlRegex.indexIn( line ) >= 0 )
	{
	    QString fileName = urlRegex.cap( 1 );
	    QString fullPath = themeDir() + fileName;
	    yuiDebug() << "Expanding " << fileName << "\tto " << fullPath << endl;
            line.replace( urlRegex, ": url(" + fullPath + ")");
	}

        if ( backgroundRegex.exactMatch( line ) )
        {
            QStringList name = backgroundRegex.cap( 1 ).split( '#' );
	    QString fullPath =  themeDir() + backgroundRegex.cap( 2 );
	    yuiDebug() << "Expanding background " << name[0] << "\tto " << fullPath << endl;

            _backgrounds[ name[0] ].filename = fullPath;
            _backgrounds[ name[0] ].full = false;

            if ( name.size() > 1 )
                _backgrounds[ name[0] ].full = ( name[1] == "full" );
        }

        if ( richTextRegex.exactMatch( line ) )
        {
            QString filename = richTextRegex.cap( 1 );
            QFile file( themeDir() + "/" + filename );

            if ( file.open(  QIODevice::ReadOnly ) )
            {
		yuiDebug() << "Reading " << file.fileName();
                _textStyle = file.readAll();
            }
	    else
	    {
		yuiError() << "Can't read " << file.fileName();
	    }
        }

        result += line;
    }

    text = result;
}


QString
QY2Styler::themeDir() const
{
    return QString( YSettings::themeDir().c_str() );
}


void
QY2Styler::registerWidget( QWidget * widget )
{
    widget->installEventFilter( this );
    widget->setAutoFillBackground( true );
    widget->setStyleSheet( _style );
    _registered_widgets.push_back( widget );
}


void
QY2Styler::unregisterWidget( QWidget * widget )
{
    _children.remove( widget );
    _registered_widgets.removeOne( widget );
}


void
QY2Styler::registerChildWidget( QWidget * parent, QWidget * widget )
{
    // Don't use yuiDebug() here - deadlock (reason unknown so far) in thread handling!

    // qDebug() << "Registering " << widget << " for parent " << parent << "\n";
    widget->installEventFilter( this );
    _children[parent].push_back( widget );
}


QImage
QY2Styler::getScaled( const QString & name, const QSize & size )
{
    QImage image = _backgrounds[ name ].pix;

    if ( size != image.size() )
        image = image.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    else
        image = image.convertToFormat( QImage::Format_ARGB32 );

    if ( image.isNull() )
	yuiError() << "Can't load pixmap from " <<  name << endl;
#if 1
    else
	yuiMilestone() << "Loaded pixmap from \"" << name
		       << "\"  size: " << image.size().width() << "x" << image.size().height()
		       << endl;
#endif

    return image;
}


void
QY2Styler::renderParent( QWidget * widget )
{
    // yuiDebug() << "Rendering " << widget << endl;
    QString name = widget->objectName();

    // TODO
    widget->setPalette( QApplication::palette() );

    // if the parent does not have a background, this does not make sense
    if ( _backgrounds[name].pix.isNull() )
        return;

    QRect fillRect = widget->contentsRect();
    if ( _backgrounds[name].full )
        fillRect = widget->rect();

    QImage back;

    if ( _backgrounds[name].lastscale != fillRect.size() )
    {
        _backgrounds[name].scaled = getScaled( name, fillRect.size() );
        _backgrounds[name].lastscale = fillRect.size();
    }

    back = _backgrounds[name].scaled;

    QPainter painter( &back );
    QWidget *child;


    foreach ( child, _children[ widget ] )
    {
        // yuiDebug() << "foreach " << child << " " << widget << endl;
        QString name = child->objectName();

        if ( ! child->isVisible() || _backgrounds [name ].pix.isNull() )
            continue;

        QRect fillRect = child->contentsRect();

        if ( _backgrounds[name].full )
            fillRect = child->rect();

        QString key = QString( "style_%1_%2_%3" ).arg( name ).arg( fillRect.width() ).arg( fillRect.height() );
        QPixmap scaled;

        if ( QPixmapCache::find( key, &scaled ) )
        {
            // yuiDebug() << "found " << key << endl;
        }
	else
	{
            scaled = QPixmap::fromImage( getScaled( name, fillRect.size() ) );
            QPixmapCache::insert( key, scaled );
        }
        painter.drawPixmap( widget->mapFromGlobal( child->mapToGlobal( fillRect.topLeft() ) ), scaled );
    }

    QPixmap result = QPixmap::fromImage( back );

    QPalette p = widget->palette();
    p.setBrush( QPalette::Window, result );
    widget->setPalette( p );
}


bool
QY2Styler::updateRendering( QWidget * widget )
{
    if ( ! widget )
        return false;

    QString name = widget->objectName();

    if (! widget->isVisible() || !widget->updatesEnabled() )
        return false;

    if ( _backgrounds[name].pix.isNull() )
    {
        QString back = _backgrounds[ name ].filename;

        if ( back.isEmpty() )
        {
            _backgrounds[ name ].pix = QImage();
        }
        else
        {
            QImage image( back );
            _backgrounds[ name ].pix = image;

	    if ( image.isNull() )
	    {
		yuiError() << "Couldn't load background image \"" << back
			   << "\" for \"" << name << "\""
			   << endl;
	    }
	    else
	    {
		yuiDebug() << "Loading background image \"" << back
			   << "\" for " << name << "\""
			   << endl;
	    }
	}
    }


    // if it's a child itself, we have to do the full blow action

    if ( !_children.contains( widget ) )
    {
        QWidget *parent = widget->parentWidget();

        while ( parent && !_children.contains( parent ) )
        {
            parent = parent->parentWidget();
        }

        if ( ! parent)
            return false;

        renderParent( parent );
    }
    else
    {
        renderParent( widget );
    }

    return true;
}


bool
QY2Styler::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize ||
	 ev->type() == QEvent::Show   ||
	 ev->type() == QEvent::LayoutRequest ||
	 ev->type() == QEvent::UpdateRequest )
    {
        updateRendering( qobject_cast<QWidget*>( obj ) );
    }

    return QObject::eventFilter( obj, ev );
}


QStringList
QY2Styler::allStyleSheets()
{
    QDir dir( themeDir(),       // path
              "*.qss",          // nameFilter
              QDir::Name,       // sort
              QDir::Files );    // filters

    return dir.entryList();
}


//
//---------------------------------------------------------------------------
//


std::ostream &
operator<<( std::ostream & stream, const QString & str )
{
    return stream << qPrintable( str );
}


std::ostream &
operator<<( std::ostream & stream, const QStringList & strList )
{
    stream << "[ ";

    for ( QStringList::const_iterator it = strList.begin();
	  it != strList.end();
	  ++it )
    {
	stream << qPrintable( *it ) << " ";
    }

    stream << " ]";

    return stream;
}


std::ostream &
operator<<( std::ostream & stream, const QWidget * widget )
{
#if LOGGING_CAUSES_QT4_THREADING_PROBLEMS

    // QObject::metaObject()->className() and QObject::objectName() can cause
    // YQUI to hang, probably due to threading problems.

    stream << "QWidget at " << hex << (void *) widget << dec;
#else
    if ( widget )
    {
	if ( widget->metaObject() )
	    stream << widget->metaObject()->className();
	else
	    stream << "<QWidget>";

	if ( ! widget->objectName().isEmpty() )
	    stream << " \"" << qPrintable( widget->objectName() ) << "\"";

	stream << " at " << hex << widget << dec;
    }
    else // ! widget
    {
	stream << "<NULL QWidget>";
    }
#endif


    return stream;
}
