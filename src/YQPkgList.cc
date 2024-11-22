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


#include <QAction>
#include <QFontMetrics>
#include <QHeaderView>
#include <QMenu>
#include <QTreeView>

#include "Exception.h"
#include "Logger.h"
#include "QY2CursorHelper.h"
#include "YQIconPool.h"
#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgList.h"


#define SINGLE_VERSION_COL	1
#define STATUS_ICON_SIZE	16
#define STATUS_COL_WIDTH        28


YQPkgList::YQPkgList( QWidget * parent )
    : YQPkgObjList( parent )
{
    resetOptimalColumnWidthValues();

    int numCol = 0;
    QStringList headers;
    QString 	versionHeaderText;
    QString 	instVersionHeaderText;


    headers <<  "";			_statusCol	= numCol++;
    headers <<  _( "Package" 	);	_nameCol	= numCol++;
    headers <<  _( "Summary" 	);	_summaryCol	= numCol++;

    if ( haveInstalledPkgs() )
    {
#if SINGLE_VERSION_COL
        versionHeaderText = _("Installed (Available)");
        headers << versionHeaderText;	_instVersionCol = numCol++;
	_versionCol = _instVersionCol;
#else
	versionHeaderText = _( "Avail. Ver." );
	headers << versionHeaderText;	_versionCol	= numCol++;

	instVersionHeaderText = _( "Inst. Ver."  );
	headers << instVersionHeaderText;  _instVersionCol = numCol++;
#endif
    }
    else
    {
	versionHeaderText = _( "Version" );
	headers << versionHeaderText;	_versionCol	= numCol++;
    }

    headers <<  _( "Size" 	);	_sizeCol	= numCol++;

    setHeaderLabels( headers );

    header()->setSortIndicatorShown( true );
    header()->setSectionsClickable( true );

    sortByColumn( statusCol(), Qt::AscendingOrder );
    setAllColumnsShowFocus( true );
    setIconSize( QSize( STATUS_ICON_SIZE, STATUS_ICON_SIZE ) );

    header()->setSectionResizeMode( QHeaderView::Interactive );

    /* NOTE: resizeEvent() is automatically triggered afterwards => sets initial column widths */

    connect ( header(), SIGNAL( sectionClicked (int) ),
	      this,	SLOT( sortByColumn (int) ) );

}


YQPkgList::~YQPkgList()
{
    // NOP
}


void YQPkgList::addPkgItem( ZyppSel selectable,
			    ZyppPkg zyppPkg )
{
    addPkgItem( selectable, zyppPkg, false );
}


void YQPkgList::addPkgItemDimmed( ZyppSel selectable,
				  ZyppPkg zyppPkg )
{
    addPkgItem( selectable, zyppPkg, true );
}


void
YQPkgList::addPkgItem( ZyppSel	selectable,
		       ZyppPkg 	zyppPkg,
		       bool 	dimmed )
{
    scrollToTop();
    scheduleDelayedItemsLayout();

    if ( ! selectable )
    {
	logError() << "NULL zypp::ui::Selectable!" << endl;
	return;
    }

    YQPkgListItem * item = new YQPkgListItem( this, selectable, zyppPkg );
    Q_CHECK_PTR( item );

    updateOptimalColumnWidthValues(selectable, zyppPkg);
    optimizeColumnWidths();

    item->setDimmed( dimmed );
    applyExcludeRules( item );
}


bool
YQPkgList::haveInstalledPkgs()
{
    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	if ( (*it)->installedObj() )
	    return true;
    }

    return false;
}


QSize
YQPkgList::sizeHint() const
{
    return QSize( 600, 350 );
}


void
YQPkgList::resetOptimalColumnWidthValues()
{
    _optimalColWidth_statusIcon = 0;
    _optimalColWidth_name = 0;
    _optimalColWidth_summary = 0;
    _optimalColWidth_version = 0;
    _optimalColWidth_instVersion = 0;
    _optimalColWidth_size = 0;
}


void
YQPkgList::updateOptimalColumnWidthValues(ZyppSel selectable, ZyppPkg zyppPkg)
{
    QString qstr;
    int qstr_width = 0;
    QFontMetrics fm( this->font() );
    const ZyppObj candidate = selectable->candidateObj();
    const ZyppObj installed = selectable->installedObj();
    // Status icon:
    _optimalColWidth_statusIcon = STATUS_COL_WIDTH;
    // Name:
    qstr = QString::fromUtf8( zyppPkg->name().c_str() );
    qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
    if (qstr_width > _optimalColWidth_name)
        _optimalColWidth_name = qstr_width;
    // Summary:
    qstr = QString::fromUtf8( zyppPkg->summary().c_str() );
    qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
    if (qstr_width > _optimalColWidth_summary)
        _optimalColWidth_summary = qstr_width;
    // Version(s):
    if ( instVersionCol() == versionCol() )	// combined column, version string
    {
        if (installed)
            qstr = QString::fromUtf8( installed->edition().c_str() );
	else
	    qstr.clear();
        if (candidate && (!installed || (candidate->edition() != installed->edition())))
        {
            if (installed)
                qstr += " ";
            qstr += "(" + QString::fromUtf8( candidate->edition().c_str() ) + ")";
        }
        qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
        if (qstr_width > _optimalColWidth_version)
            _optimalColWidth_version = qstr_width;
    }
    else	// separate columns, version strings
    {
        if (candidate)
        {
            qstr = QString::fromUtf8( candidate->edition().c_str() );
            qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
            if (qstr_width > _optimalColWidth_version)
                _optimalColWidth_version = qstr_width;
        }
        if (installed)
        {
            qstr = QString::fromUtf8( installed->edition().c_str() );
            qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
            if (qstr_width > _optimalColWidth_instVersion)
                _optimalColWidth_instVersion = qstr_width;
        }
    }
    // Size:
    qstr = QString::fromUtf8( zyppPkg->installSize().asString().c_str() );
    qstr_width = fm.boundingRect( qstr ).width() + ( STATUS_ICON_SIZE / 2 );
    if (qstr_width > _optimalColWidth_size)
        _optimalColWidth_size = qstr_width;
}


void
YQPkgList::optimizeColumnWidths()
{
    int visibleSpace = 0;
    int optimalWidthsSum = 0;    // Sum of all optimal (sized-to-content-) column width values
    int numOptCol = 4;       // Nr. of columns for distribution of remaining space
    int statusIconColWidth = _optimalColWidth_statusIcon;

    if (statusIconColWidth == 0)
        statusIconColWidth = STATUS_COL_WIDTH;
    optimalWidthsSum = _optimalColWidth_statusIcon + _optimalColWidth_name + _optimalColWidth_summary + _optimalColWidth_version + _optimalColWidth_size;
    if ( instVersionCol() != versionCol() )
    {
        optimalWidthsSum += _optimalColWidth_instVersion;
        numOptCol++;
    }
    // CHECK IF WE HAVE LESS VISIBLE SPACE THAN WE NEED:
    visibleSpace = this->viewport()->width();
    if (visibleSpace < 0) return;
    if (optimalWidthsSum >= visibleSpace)	// THERE IS NOT ENOUGH VISIBLE SPACE TO SHOW ALL CLOUMNS WITH OPTIMAL WIDTHS
    {
        /* ONLY REDUCE WIDTH OF THE "summary"-COLUMN BEYOND OPTIMAL WIDTH
	 * IF THIS IS NOT ENOUGH, WE WILL GET A HORIZONTAL SCROLL BAR		*/
        int reducedSummaryWidth = visibleSpace - optimalWidthsSum + _optimalColWidth_summary;
        if (reducedSummaryWidth < 100)
            reducedSummaryWidth = 100;
	// Set new column widths:
	this->setColumnWidth( statusCol(), statusIconColWidth );
	this->setColumnWidth( nameCol(), _optimalColWidth_name );
        this->setColumnWidth( summaryCol(), reducedSummaryWidth);
	this->setColumnWidth( versionCol(), _optimalColWidth_version );
	if ( instVersionCol() != versionCol() )
	    this->setColumnWidth( instVersionCol(), _optimalColWidth_instVersion );
	this->setColumnWidth( sizeCol(), _optimalColWidth_size);
    }
    else	// THERE IS ENOUGH VISIBLE SPACE
    {
        // DISTRIBUTE REMAINING VISIBLE SPACE TO ALL COLUMNS (except the satusIcon-column):
        // Calculate additional column widths:
        int addSpace = (visibleSpace - optimalWidthsSum) / numOptCol;
        int addSpaceR = (visibleSpace - optimalWidthsSum) % numOptCol;
        // Set new column widths:
        this->setColumnWidth( statusCol(), statusIconColWidth );
        this->setColumnWidth( nameCol(), _optimalColWidth_name + addSpace );
        this->setColumnWidth( summaryCol(), _optimalColWidth_summary + addSpace );
        this->setColumnWidth( versionCol(), _optimalColWidth_version + addSpace );
        if ( instVersionCol() != versionCol() )
            this->setColumnWidth( instVersionCol(), _optimalColWidth_instVersion + addSpace );
        this->setColumnWidth( sizeCol(), _optimalColWidth_size + addSpace + addSpaceR );
    }
}


void
YQPkgList::clear()
{
    YQPkgObjList::clear();
    resetOptimalColumnWidthValues();
    optimizeColumnWidths();
}


void
YQPkgList::resort()
{
    int col = sortColumn();
    Qt::SortOrder order = header()->sortIndicatorOrder();
    sortByColumn(col, order);
}


void
YQPkgList::resizeEvent(QResizeEvent *event)
{
    if (event->size().width() != event->oldSize().width())
        optimizeColumnWidths();
    /* NOTE: avoids column width optimization when the size changes
       because the horizontal scroll bar appeares/disappeares */
    event->accept();
}


void
YQPkgList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QMenu( this );
    Q_CHECK_PTR( _notInstalledContextMenu );

    _notInstalledContextMenu->addAction(actionSetCurrentInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentDontInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentTaboo);

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgList::createInstalledContextMenu()
{
    _installedContextMenu = new QMenu( this );
    Q_CHECK_PTR( _installedContextMenu );

    _installedContextMenu->addAction(actionSetCurrentKeepInstalled);
    _installedContextMenu->addAction(actionSetCurrentDelete);
    _installedContextMenu->addAction(actionSetCurrentUpdate);
    _installedContextMenu->addAction(actionSetCurrentUpdateForce);
    _installedContextMenu->addAction(actionSetCurrentProtected);

    addAllInListSubMenu( _installedContextMenu );
}


QMenu *
YQPkgList::addAllInListSubMenu( QMenu * menu )
{
    QMenu * submenu = new QMenu( menu );
    Q_CHECK_PTR( submenu );

    submenu->addAction(actionSetListInstall);
    submenu->addAction(actionSetListDontInstall);
    submenu->addAction(actionSetListKeepInstalled);
    submenu->addAction(actionSetListDelete);
    submenu->addAction(actionSetListDelete);
    submenu->addAction(actionSetListUpdate);
    submenu->addAction(actionSetListUpdateForce);
    submenu->addAction(actionSetListTaboo);
    submenu->addAction(actionSetListProtected);

    QAction *action = menu->addMenu( submenu );
    action->setText(_( "&All in This List" ));

    return submenu;
}


int
YQPkgList::globalSetPkgStatus( ZyppStatus newStatus, bool force, bool countOnly )
{
    busyCursor();
    int changedCount = 0;

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel	   selectable = *it;
	ZyppStatus oldStatus  = selectable->status();

	if ( newStatus != oldStatus )
	{
	    bool doChange = false;

	    switch ( newStatus )
	    {
		case S_KeepInstalled:
		case S_Del:
		case S_AutoDel:
		case S_Protected:
		    doChange = !selectable->installedEmpty();
		    break;

		case S_Update:
		case S_AutoUpdate:

		    if ( force )
		    {
			doChange = !selectable->installedEmpty();
		    }
		    else // don't force - update only if useful (if candidate is newer)
		    {
			const ZyppObj candidate = selectable->candidateObj();
			const ZyppObj installed = selectable->installedObj();

			if ( candidate && installed )
			{
			    doChange = ( installed->edition() < candidate->edition() );
			}
		    }
		    break;

		case S_Install:
		case S_AutoInstall:
		case S_NoInst:
		case S_Taboo:
		    doChange = selectable->installedEmpty();
		    break;
	    }

	    if ( doChange )
	    {
		if ( ! countOnly && oldStatus != S_Protected )
		    selectable->setStatus( newStatus );

		changedCount++;
		// logInfo() << "Updating " << selectable->name() << endl;
	    }
	}
    }

    if ( changedCount > 0 && ! countOnly )
    {
	emit updateItemStates();
	emit updatePackages();
	emit statusChanged();
    }

    normalCursor();

    return changedCount;
}






YQPkgListItem::YQPkgListItem( YQPkgList * 		pkgList,
			      ZyppSel	selectable,
			      ZyppPkg 	zyppPkg )
    : YQPkgObjListItem( pkgList, selectable, zyppPkg )
    , _pkgList( pkgList )
    , _zyppPkg( zyppPkg )
    , _dimmed( false )
{
    if ( ! _zyppPkg )
	_zyppPkg = tryCastToZyppPkg( selectable->theObj() );

    setTextAlignment( sizeCol(), Qt::AlignRight );
}


YQPkgListItem::~YQPkgListItem()
{
    // NOP
}


void
YQPkgListItem::updateData()
{
    YQPkgObjListItem::updateData();
}


QString
YQPkgListItem::toolTip( int col )
{
    QString text;
    QString name = _zyppObj->name().c_str();

    if ( col == statusCol() )
    {
	text = YQPkgObjListItem::toolTip( col );
    }
    else
    {
	text = name + "\n\n";

	QString installed;
	QString candidate;

	if ( !selectable()->installedEmpty() )
	{
	    installed  = selectable()->installedObj()->edition().asString().c_str();
	    installed += "-";
	    installed +=  selectable()->installedObj()->arch().asString().c_str();
	    installed  = _( "Installed Version: %1" ).arg( installed );
	}

	if (  selectable()->hasCandidateObj() )
	{
	    candidate  = selectable()->candidateObj()->edition().asString().c_str();
	    candidate += "-";
	    candidate +=  selectable()->candidateObj()->arch().asString().c_str();
	}

	if ( !selectable()->installedEmpty() )
	{
	    text += installed + "\n";

	    if ( selectable()->hasCandidateObj() )
	    {
		// Translators: This is the relation between two versions of one package
		// if both versions are the same, e.g., both "1.2.3-42", "1.2.3-42"
		QString relation = _( "same" );

		if ( _candidateIsNewer )	relation = _( "newer" );
		if ( _installedIsNewer )	relation = _( "older" );

		// Translators: %1 is the version, %2 is one of "newer", "older", "same"
		text += _( "Available Version: %1 (%2)" ).arg( candidate ).arg( relation );
	    }
	    else
	    {
		text += _( "Not available for installation" );
	    }
	}
	else // not installed
	{
	    text += candidate;
	}
    }

    return text;
}
