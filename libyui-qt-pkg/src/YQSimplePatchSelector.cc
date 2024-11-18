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
  File:	      YQSimplePatchSelector.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#define YUILogComponent "qt-pkg"
#include "Logger.h"

#include <yui/YEvent.h>
#include <yui/qt/utf8.h>
#include <yui/qt/YQUI.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/YQWizard.h>
#include <yui/qt/YQDialog.h>

#include <QApplication>
#include <QBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QSplitter>

#include "YQSimplePatchSelector.h"
#include "YQPkgConflictDialog.h"
#include "YQPkgDiskUsageList.h"
#include "YQPkgPatchFilterView.h"
#include "YQPkgPatchList.h"
#include "QY2LayoutUtils.h"


using std::max;
using std::endl;
using std::string;

#define SHOW_DISK_USAGE		0

#define SPACING			6
#define MARGIN			6



YQSimplePatchSelector::YQSimplePatchSelector( YWidget *	parent, long modeFlags )
    : YQPackageSelectorBase( parent, modeFlags )
{
    _patchFilterView	= 0;
    _patchList		= 0;
    _diskUsageList	= 0;
    _wizard		= findWizard();

    basicLayout();
    makeConnections();

#if 0
    _patchList->fillList();
    _patchList->selectSomething();
#endif

    if ( _diskUsageList )
	_diskUsageList->updateDiskUsage();
}



YQWizard *
YQSimplePatchSelector::findWizard() const
{
    YQWizard * wizard = 0;

    YQDialog * dialog = dynamic_cast<YQDialog *> ( YDialog::currentDialog() );

    if ( dialog )
	wizard = dialog->findWizard();

    return wizard;
}



void
YQSimplePatchSelector::basicLayout()
{
    QSplitter * splitter = new QSplitter( Qt::Vertical, this );
    Q_CHECK_PTR( splitter );

    setLayout( new QVBoxLayout( ) );
    layout()->addWidget(splitter);

    //
    // PatchFilterView
    //

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin( MARGIN );
    QWidget * upper_vbox = new QWidget( splitter );
    Q_CHECK_PTR( upper_vbox );
    upper_vbox->setLayout(layout);
    splitter->addWidget(upper_vbox);

    splitter->setStretchFactor( 0, 1 );

    _patchFilterView = new YQPkgPatchFilterView( upper_vbox );
    layout->addWidget( _patchFilterView );
    Q_CHECK_PTR( _patchFilterView );

    _patchList = _patchFilterView->patchList();
    Q_CHECK_PTR( _patchList );

    //addVSpacing( upper_vbox, MARGIN );

    //
    // Disk Usage
    //


#if SHOW_DISK_USAGE
    layout = new QVBoxLayout;
    QWidget * lower_vbox = new QWidget( splitter );
    lower_vbox->setLayout(layout);

    Q_CHECK_PTR( lower_vbox );
    //addVSpacing( lower_vbox, MARGIN );

    _diskUsageList = new YQPkgDiskUsageList( lower_vbox );
    Q_CHECK_PTR( _diskUsageList );
    layout->addWidget(_diskUsageList);

    splitter->setSectionResizeMode( lower_vbox, QSplitter::FollowSizeHint );
#endif


    //
    // Buttons
    //

    if ( _wizard )	// No button box - add "Details..." button here
    {
	//
	// "Details" button
	//

	//addVSpacing( this, SPACING );
        layout = new QHBoxLayout;
        QWidget * hbox = new QWidget( this );
	Q_CHECK_PTR( hbox );
        hbox->setLayout(layout);
	QPushButton * details_button = new QPushButton( _( "&Details..." ), hbox );
        layout->addWidget(details_button);
	Q_CHECK_PTR( details_button );

	connect( details_button, SIGNAL( clicked() ),
		 this,		 SLOT  ( detailedPackageSelection() ) );

	//addHStretch( hbox );
    }
    else // ! _wizard
    {
	layoutButtons( this );
    }
}


void
YQSimplePatchSelector::layoutButtons( QWidget * parent )
{
    QWidget * button_box = new QWidget( parent );
    QHBoxLayout *layout = new QHBoxLayout( button_box );
    Q_CHECK_PTR( button_box );
    layout->setMargin ( MARGIN  );
    layout->setSpacing( SPACING );


    QPushButton * details_button = new QPushButton( _( "&Details..." ), button_box );
    Q_CHECK_PTR( details_button );
    layout->addWidget(details_button);
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

    this->layout()->addWidget( button_box );
//    button_box->setFixedHeight( button_box->sizeHint().height() );
}



void
YQSimplePatchSelector::makeConnections()
{
    if ( _patchList && _diskUsageList )
    {
	connect( _patchList,	 SIGNAL( updatePackages()  ),
		 _diskUsageList, SLOT  ( updateDiskUsage() ) );
    }

    yuiMilestone() << "Connection set up" << endl;

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
YQSimplePatchSelector::detailedPackageSelection()
{
    yuiMilestone() << "\"Details..\" button clicked" << endl;
    YQUI::ui()->sendEvent( new YMenuEvent( "details" ) );
}


void
YQSimplePatchSelector::debugTrace()
{
    yuiWarning() << "debugTrace" << endl;
}



