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
  File:	      YQPatternSelector.cc
  See also:   YQPatternSelectorHelp.cc

  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/YEvent.h>
#include <yui/qt/YQUI.h>
#include <yui/qt/YQDialog.h>
#include <yui/qt/YQWizard.h>
#include "YQi18n.h"
#include <yui/qt/utf8.h>

#include <QApplication>
#include <QHeaderView>
#include <QPushButton>
#include <QSplitter>
#include <QLayout>

#include "YQPatternSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgSelDescriptionView.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgPatternList.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


#define ALWAYS_SOLVE_IMMEDIATELY 1

using std::max;
using std::string;

#define SPACING			6
#define MARGIN			6


YQPatternSelector::YQPatternSelector( YWidget *	parent, long modeFlags )
    : YQPackageSelectorBase( parent, modeFlags )
{
    _patternList		= 0;
    _descriptionView		= 0;
    _wizard			= findWizard();

    basicLayout();
    makeConnections();

    if ( _patternList )
    {
	_patternList->fillList();
	_patternList->selectSomething();
    }

    if ( zyppPool().empty<zypp::Pattern  >() )
    {
	logWarning() << "Neither patterns nor selections in ZyppPool" << endl;
    }


    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();
}



YQWizard *
YQPatternSelector::findWizard() const
{
    YQWizard * wizard = 0;

    YQDialog * dialog = dynamic_cast<YQDialog *> ( YDialog::currentDialog() );

    if ( dialog )
	wizard = dialog->findWizard();

    return wizard;
}



void
YQPatternSelector::basicLayout()
{
    QVBoxLayout *vbox = new QVBoxLayout();
    setLayout(vbox);

    QSplitter * outer_splitter = new QSplitter( Qt::Horizontal, this );
    Q_CHECK_PTR( outer_splitter );

    vbox->addWidget( outer_splitter );

    QWidget * left_pane	 = layoutLeftPane ( outer_splitter );
    QWidget * right_pane = layoutRightPane( outer_splitter );

    int left_pane_width = (int) ( 0.3 * YQUI::ui()->defaultSize( YD_HORIZ ) );
    left_pane->resize( QSize( left_pane_width, left_pane->height() ) );

    outer_splitter->setStretchFactor(outer_splitter->indexOf(left_pane), 0);
    outer_splitter->setStretchFactor(outer_splitter->indexOf(right_pane), 1);

    if ( ! _wizard )
	layoutButtons( this );
}



QWidget *
YQPatternSelector::layoutLeftPane( QWidget * parent )
{
    QWidget *vbox = new QWidget(parent);
    QVBoxLayout * layout = new QVBoxLayout( vbox );
    Q_CHECK_PTR( vbox );
    layout->setMargin( MARGIN );
    vbox->setLayout(layout);

    if ( ! zyppPool().empty<zypp::Pattern>() )
    {
	//
	// Patterns list
	//

	_patternList = new YQPkgPatternList( vbox,
					     false,	// no autoFill - need to connect to details view first
					     false );	// no autoFilter - filterMatch() is not connected
	Q_CHECK_PTR( _patternList );
        layout->addWidget(_patternList);
        //_patternList->header()->hide();
    }

    if ( _wizard )	// No button box - add "Details..." button here
    {
	//
	// "Details" button
	//

	layout->addSpacing( SPACING );

	QHBoxLayout * hbox = new QHBoxLayout();
	Q_CHECK_PTR( hbox );
        layout->addLayout(hbox);

	QPushButton * details_button = new QPushButton( _( "&Details..." ), vbox );
	Q_CHECK_PTR( details_button );
        hbox->addWidget(details_button);

	connect( details_button, SIGNAL( clicked() ),
		 this,		 SLOT  ( detailedPackageSelection() ) );

	hbox->addStretch();
    }

    return vbox;
}



QWidget *
YQPatternSelector::layoutRightPane( QWidget * parent )
{
    QSplitter * splitter = new QSplitter( Qt::Vertical, parent );
    Q_CHECK_PTR( splitter );
    //splitter->setMargin( MARGIN );


    //
    // Selection / Pattern description
    //
    QWidget *upper_vbox = new QWidget(splitter);
    QVBoxLayout * layout = new QVBoxLayout(upper_vbox);

    Q_CHECK_PTR( upper_vbox );

    _descriptionView = new YQPkgSelDescriptionView( upper_vbox );
    Q_CHECK_PTR( _descriptionView );
    layout->addWidget(_descriptionView);

    //
    // Disk usage
    //

    QWidget *lower_vbox = new QWidget(splitter);
    layout = new QVBoxLayout( lower_vbox);

    Q_CHECK_PTR( lower_vbox );

    _diskUsageList = new YQPkgDiskUsageList( lower_vbox );
    Q_CHECK_PTR( _diskUsageList );
    layout->addWidget(_diskUsageList);

    splitter->setStretchFactor( 0, 3 );
    splitter->setStretchFactor( 0, 1 );

    return splitter;
}



void
YQPatternSelector::layoutButtons( QWidget * parent )
{
    QWidget *button_box = new QWidget(parent);
    Q_CHECK_PTR( button_box );

    parent->layout()->addWidget( button_box );
    QHBoxLayout *layout = new QHBoxLayout(button_box);

    layout->setMargin ( MARGIN  );
    layout->setSpacing( SPACING );

    QPushButton * details_button = new QPushButton( _( "&Details..." ), button_box );
    layout->addWidget(details_button);
    Q_CHECK_PTR( details_button );
    details_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( details_button,	SIGNAL( clicked() ),
	     this,		SLOT  ( detailedPackageSelection() ) );


    layout->addStretch();

    QPushButton * cancel_button = new QPushButton( _( "&Cancel" ), button_box );
    Q_CHECK_PTR( cancel_button );
    layout->addWidget(cancel_button);
    cancel_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( cancel_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( reject()   ) );


    QPushButton * accept_button = new QPushButton( _( "&Accept" ), button_box );
    Q_CHECK_PTR( accept_button );
    layout->addWidget(accept_button);
    accept_button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert

    connect( accept_button, SIGNAL( clicked() ),
	     this,	    SLOT  ( accept()   ) );

    button_box->setFixedHeight( button_box->sizeHint().height() );
}



void
YQPatternSelector::makeConnections()
{
    if ( _patternList )
    {
#if ALWAYS_SOLVE_IMMEDIATELY
	connect( _patternList,		SIGNAL( statusChanged()		),
		 this,			SLOT  ( resolveDependencies()	) );

	if ( _pkgConflictDialog )
	{
	    connect( _pkgConflictDialog, SIGNAL( updatePackages()	),
		     _patternList,	 SLOT  ( updateItemStates()	) );
	}
#endif

	if ( _descriptionView )
	{
	    connect( _patternList,	SIGNAL( currentItemChanged( ZyppSel ) ),
		     _descriptionView,	SLOT  ( showDetails	( ZyppSel ) ) );
	}

	if ( _diskUsageList )
	{
	    connect( _patternList,	SIGNAL( updatePackages()  ),
		     _diskUsageList,	SLOT  ( updateDiskUsage() ) );
	}

    }

    logInfo() << "Connection set up" << endl;


    if ( _wizard )
    {
	connect( _wizard, 	SIGNAL( nextClicked()	),
		 this,		SLOT  ( accept()        ) );

	connect( _wizard, 	SIGNAL( backClicked()	),
		 this,		SLOT  ( reject()	) );

	connect( _wizard, 	SIGNAL( abortClicked()	),
		 this,		SLOT  ( reject()	) );
    }
}


void
YQPatternSelector::detailedPackageSelection()
{
    logInfo() << "\"Details..\" button clicked" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "details" ) );
}


void
YQPatternSelector::debugTrace()
{
    logWarning() << "debugTrace" << endl;
}



