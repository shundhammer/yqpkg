/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QComboBox>
#include <QElapsedTimer>
#include <QLabel>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

#include <zypp/Patch.h>

#include "Exception.h"
#include "FSize.h"
#include "Logger.h"
#include "QY2LayoutUtils.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgPatchList.h"
#include "YQi18n.h"

#include "YQPkgPatchFilterView.h"


typedef zypp::Patch::Contents			ZyppPatchContents;
typedef zypp::Patch::Contents::const_iterator	ZyppPatchContentsIterator;

using std::set;

#define ENABLE_TOTAL_DOWNLOAD_SIZE	0

YQPkgPatchFilterView::YQPkgPatchFilterView( QWidget * parent )
    : QWidget( parent )
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    _splitter			= new QSplitter( Qt::Vertical, this );	Q_CHECK_PTR( _splitter 	);
    layout->addWidget(_splitter);

    QWidget *upper_box = new QWidget( _splitter );
    QVBoxLayout *vbox = new QVBoxLayout( upper_box );
    _patchList			= new YQPkgPatchList( upper_box );
    Q_CHECK_PTR( _patchList 	);

    vbox->addWidget( _patchList );

    QHBoxLayout * hbox 		= new QHBoxLayout(); Q_CHECK_PTR( hbox );
    vbox->addLayout(hbox);
    vbox->setContentsMargins(0,0,0,0);


    QLabel * label		= new QLabel( _( "&Show Patch Category:" ), upper_box );
    hbox->addWidget(label);

    _patchFilter		= new QComboBox( upper_box );
    Q_CHECK_PTR( _patchFilter );
    hbox->addWidget(_patchFilter);

    _patchFilter->addItem( _( "Needed Patches" ));
    _patchFilter->addItem( _( "Unneeded Patches" ));
    _patchFilter->addItem( _( "All Patches" ),				2 );
    _patchFilter->setCurrentIndex( 0 );

    label->setBuddy( _patchFilter );

    connect( _patchFilter, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
             this, &YQPkgPatchFilterView::fillPatchList );

    _detailsViews		= new QTabWidget( _splitter );			Q_CHECK_PTR( _detailsViews	);

    _descriptionView		= new YQPkgDescriptionView( _detailsViews );	Q_CHECK_PTR( _descriptionView	);

    _detailsViews->addTab( _descriptionView, _( "Patch Description" ) );

    connect( _patchList,	SIGNAL( currentItemChanged    ( ZyppSel ) ),
	     _descriptionView,	SLOT  ( showDetailsIfVisible( ZyppSel ) ) );

    connect( _patchList,	SIGNAL( statusChanged() 		),
	     this,		SLOT  ( updateTotalDownloadSize() 	) );

    updateTotalDownloadSize();
}


YQPkgPatchFilterView::~YQPkgPatchFilterView()
{
    // NOP
}


void
YQPkgPatchFilterView::updateTotalDownloadSize()
{
    set<ZyppSel> selectablesToInstall;
    QElapsedTimer calcTime;
    calcTime.start();

    for ( ZyppPoolIterator patches_it = zyppPatchesBegin();
	  patches_it != zyppPatchesEnd();
	  ++patches_it )
    {
	ZyppPatch patch = tryCastToZyppPatch( (*patches_it)->theObj() );

	if ( patch )
	{
	    ZyppPatchContents patchContents( patch->contents() );

	    for ( ZyppPatchContentsIterator contents_it = patchContents.begin();
		  contents_it != patchContents.end();
		  ++contents_it )
	    {
		ZyppPkg pkg =  zypp::make<zypp::Package>(*contents_it);
		ZyppSel sel;

		if ( pkg )
		    sel = _selMapper.findZyppSel( pkg );

		if ( sel )
		{
		    switch ( sel->status() )
		    {
			case S_Install:
			case S_AutoInstall:
			case S_Update:
			case S_AutoUpdate:
			    // Insert the patch contents selectables into a set,
			    // don't immediately sum up their sizes: The same
			    // package could be in more than one patch, but of
			    // course it will be downloaded only once.

			    selectablesToInstall.insert( sel );
			    break;

			case S_Del:
			case S_AutoDel:
			case S_NoInst:
			case S_KeepInstalled:
			case S_Taboo:
			case S_Protected:
			    break;

			    // intentionally omitting 'default' branch so the compiler can
			    // catch unhandled enum states
		    }

		}
	    }
	}
    }


    FSize totalSize = 0;

    for ( set<ZyppSel>::iterator it = selectablesToInstall.begin();
	  it != selectablesToInstall.end();
	  ++it )
    {
	if ( (*it)->candidateObj() )
	    totalSize += zypp::ByteCount::SizeType((*it)->candidateObj()->installSize());
    }

#if ENABLE_TOTAL_DOWNLOAD_SIZE
    _totalDownloadSize->setText( totalSize.asString().c_str() );
#endif

    logDebug() << "Calculated total download size in "
	       << calcTime.elapsed() << " millisec"
	       << endl;
}


void
YQPkgPatchFilterView::fillPatchList()
{
    switch ( _patchFilter->currentIndex() )
    {
	case 0:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
	case 1:		_patchList->setFilterCriteria( YQPkgPatchList::RelevantAndInstalledPatches );	break;
	case 2:		_patchList->setFilterCriteria( YQPkgPatchList::AllPatches		   );	break;
	default:	_patchList->setFilterCriteria( YQPkgPatchList::RelevantPatches		   );	break;
    }

    _patchList->fillList();
    _patchList->selectSomething();
}


