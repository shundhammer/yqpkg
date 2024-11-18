/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


/*
  File:	      YQPkgConflictList.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/qt/YQUI.h>
#include <yui/qt/YQApplication.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <zypp/ZYppFactory.h>

#include <errno.h>

#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QMessageBox>
#include <QRadioButton>
#include <QList>
#include <QToolTip>
#include <QDebug>
#include <QVBoxLayout>

#include "YQPkgConflictList.h"
#include "YQPkgConflictDialog.h"
#include "YQIconPool.h"

#include "Logger.h"
#include "Exception.h"


using std::list;
using std::string;


#define LIST_SPLIT_THRESHOLD	8

#define RED			QColor( 0xC0, 0, 0 )
#define BRIGHT_RED		QColor( 0xFF, 0, 0 )
#define BLUE			QColor( 0, 0, 0xC0 )
#define LIGHT_BLUE		QColor( 0xE0, 0xE0, 0xF8 )
#define LIGHT_GREY		QColor( 0xE0, 0xE0, 0xE0 )
#define MAGENTA			Qt::magenta
#define DEEP_ORANGE		QColor( 0xFF, 0x80, 0x20 )
#define LIGHT_ORANGE		QColor( 0xFF, 0xC0, 0x50 )


YQPkgConflictList::YQPkgConflictList( QWidget * parent )
    : QScrollArea( parent ), _layout( 0 )
{
    setWidget( new QFrame( this ) );
    _layout = new QVBoxLayout;
    widget()->setLayout( _layout );
    clear();

    widget()->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
}


YQPkgConflictList::~YQPkgConflictList()
{
    // NOP
}

void
YQPkgConflictList::clear()
{
    YQPkgConflict * conflict;
    
    foreach( conflict, _conflicts )
    {
        _layout->removeWidget( conflict );
        delete conflict;
    }
    _conflicts.clear();
    
    // kill the stretch item, too
    delete _layout->takeAt( 0 );
}

void
YQPkgConflictList::fill( zypp::ResolverProblemList problemList )
{
    clear();
    string text;

    zypp::ResolverProblemList::iterator it = problemList.begin();

    while ( it != problemList.end() )
    {
	YQPkgConflict *conflict = new YQPkgConflict( widget(), *it );
	Q_CHECK_PTR( conflict );

        connect( conflict, SIGNAL( expanded() ),
                 SLOT( relayout() ) );
        _layout->addWidget( conflict );
        _conflicts.push_back( conflict );
	++it;
    }
    _layout->addStretch( 1 );
    relayout();
}

void YQPkgConflictList::relayout()
{
     // for some weird reason, the layout's minSize is still 18x18 even after 3000 pixels
    // inserted, so we have to do the math on our own
    QSize minSize = QSize( _layout->margin() * 2, _layout->margin() * 2 );

    YQPkgConflict * conflict;
    
    foreach( conflict, _conflicts )
    {
        minSize = minSize.expandedTo( conflict->minimumSizeHint() );
        minSize.rheight() += conflict->minimumSizeHint().height() + _layout->spacing();
    }

    widget()->resize( minSize );
}

void
YQPkgConflictList::applyResolutions()
{
    zypp::ProblemSolutionList userChoices;
    YQPkgConflict *           conflict;
    
    foreach( conflict, _conflicts )
    {
        zypp::ProblemSolution_Ptr userChoice = conflict->userSelectedResolution();

        if ( userChoice )
            userChoices.push_back( userChoice );
    }

    zypp::getZYpp()->resolver()->applySolutions( userChoices );
    emit updatePackages();
}


void
YQPkgConflictList::askSaveToFile() const
{
    QString filename = YQApplication::askForSaveFileName( "conflicts.txt",	// startsWith
							  "*.txt",		// filter
							  _( "Save Conflicts List" ) );
    if ( ! filename.isEmpty() )
	saveToFile( filename, true );
}


void
YQPkgConflictList::saveToFile( const QString filename, bool interactive ) const
{
    // Open file
    QFile file(filename);

    if ( ! file.open(QIODevice::WriteOnly) )
    {
	logError() << "Can't open file " << filename << endl;

	if ( interactive )
	{
	    // Post error popup.

	    QMessageBox::warning( 0,						// parent
				  _( "Error" ),					// caption
				  _( "Cannot open file %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
	return;
    }


    // Write header

    QString header = "#### YaST2 conflicts list - generated ";
    header += QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" );
    header += " ####\n\n";

    file.write(header.toUtf8());

    YQPkgConflict * conflict;
    
    foreach( conflict, _conflicts )
    {
	conflict->saveToFile( file );
    }


    // Write footer

    file.write("\n#### YaST2 conflicts list END ###\n" );


    // Clean up

    if ( file.isOpen() )
	file.close();
}






YQPkgConflict::YQPkgConflict( QWidget *				parent,
			      zypp::ResolverProblem_Ptr		problem	)
    : QFrame( parent )
    , _problem( problem )
    , _resolutionsHeader( 0 )
{
    _layout = new QVBoxLayout( this );
    _layout->setSpacing( 0 );
    _layout->setMargin( 0 );
    
    formatHeading();
    
    QLabel * detailsLabel = new QLabel( fromUTF8 ( _problem->details() ), this );
    _layout->addWidget( detailsLabel );

    setProperty( "class", "conflict" );
    addSolutions();
    setMinimumSize( _layout->minimumSize() );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}


void
YQPkgConflict::formatHeading()
{
    QFrame * frame = new QFrame( this );
    frame->setProperty( "class", "conflict-frame" );
    
    QHBoxLayout * hbox  = new QHBoxLayout( frame );

    QLabel * pix = new QLabel( this );
    pix->setPixmap( YQIconPool::normalPkgConflict() );
    hbox->addWidget( pix );

    QString  text = fromUTF8( problem()->description() );
    QLabel * heading = new QLabel( text, this );
    heading->setProperty( "class", "conflict-heading" );
    heading->setStyleSheet( "font-size: +2; font: bold;" );
    hbox->addWidget( heading );
    
    hbox->addStretch( 1 );

    _layout->addWidget( frame );
}


void
YQPkgConflict::addSolutions()
{
    _resolutionsHeader = new QLabel( _( "Conflict Resolution:" ), this );
    _layout->addWidget( _resolutionsHeader );

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addSpacing( 20 );

    QVBoxLayout *vbox = new QVBoxLayout();
    hbox->addLayout( vbox );
    _layout->addLayout( hbox );

    zypp::ProblemSolutionList solutions = problem()->solutions();
    zypp::ProblemSolutionList::iterator it = solutions.begin();

    int n=0;

    while ( it != solutions.end() )
    {
        ++n;
        QString shortcut = "" + QString( (n<10)?"&":"" ) +  QString::number(n) + ": ";

        QRadioButton * solutionButton = new QRadioButton( shortcut +  fromUTF8( ( *it )->description() ), this );
        vbox->addWidget( solutionButton );
        _solutions[ solutionButton ] = *it;

        QString details = fromUTF8( ( *it )->details() );
	
        if ( ! details.isEmpty() )
        {
            QStringList lines = details.split( "\n" );
	    
            if ( lines.count() > 7 )
            {
                details = "<qt>";
		
                for ( int i = 0; i < 4; i++ )
                    details += lines[i] + "<br>\n";
		
                details += _( "<a href='/'>%1 more...</a>" ).arg( lines.count() - 4 );
            }
	    
            QLabel * detailsLabel = new QLabel( details, this );
	    
            connect( detailsLabel, 	SIGNAL( linkActivated ( const QString & ) ),
                     this,		SLOT  ( detailsExpanded()                 ) );

            QHBoxLayout * hbox = new QHBoxLayout();
            hbox->addSpacing( 15 );
            hbox->addWidget( detailsLabel );
            vbox->addLayout( hbox );
            _details[ detailsLabel ] = *it;
        }
	
	++it;
    }
}

void
YQPkgConflict::detailsExpanded()
{
    QLabel * obj = qobject_cast<QLabel*>( sender() );
    
    if ( !obj || ! _details.contains( obj ) )
        return;

    QSize _size = size();
    int oldHeight = obj->height();
    obj->setText( fromUTF8( _details[obj]->details() ) );

    resize( _size.width(), _size.height() + ( obj->minimumSizeHint().height() - oldHeight ) );
    emit expanded();
}


zypp::ProblemSolution_Ptr
YQPkgConflict::userSelectedResolution()
{
    QMap<QRadioButton*, zypp::ProblemSolution_Ptr>::iterator it;

    for ( it = _solutions.begin(); it != _solutions.end(); ++it )
    {
        QRadioButton *button = it.key();
        if ( !button->isChecked() )
            continue;
        zypp::ProblemSolution_Ptr solution = it.value();

        logInfo() << "User selected resolution \""<< solution->description()
		       <<"\"" << endl;
        return solution;
    }

    return zypp::ProblemSolution_Ptr();		// Null pointer
}


void
YQPkgConflict::saveToFile( QFile &file ) const
{
    if ( ! file.isOpen() )
	return;

    // Write item

    QMap<QRadioButton*, zypp::ProblemSolution_Ptr>::const_iterator it;

    file.write( problem()->description().c_str() );
    file.write( "\n" );
    file.write( problem()->details().c_str() );
    file.write( "\n" );

    QString buffer;

    for ( it = _solutions.begin(); it != _solutions.end(); ++it )
    {
        QRadioButton  *button = it.key();
        zypp::ProblemSolution_Ptr solution = it.value();
        buffer = QString( "    [%1] %2\n" )
            .arg( button->isChecked() ? "x" : " " )
            .arg( qPrintable( fromUTF8( solution->description() ) ) );
        buffer += fromUTF8( solution->details() );
	buffer += "\n";
        file.write( buffer.toUtf8() );
    }
    
    file.write( "\n\n" );
}

