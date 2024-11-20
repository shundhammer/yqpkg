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
  File:	      YQPkgRepoList.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <QElapsedTimer>
#include <QHeaderView>
#include <QTreeWidget>

#include <zypp/RepoManager.h>
#include <zypp/PoolQuery.h>

#include "Logger.h"
#include "QY2CursorHelper.h"
#include "QY2IconLoader.h"
#include "YQPkgFilters.h"
#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgRepoList.h"

#define TIME_FILTER     0


using std::string;
using std::list;
using std::set;
using std::vector;


YQPkgRepoList::YQPkgRepoList( QWidget * parent )
    : QY2ListView( parent )
{
    // logVerbose() << "Creating repository list" << endl;

    _nameCol	= -1;
    _urlCol	= -1;

    int numCol = 0;

    QStringList headers;

    // Column headers for repository list
    headers <<  _( "Name");	_nameCol	= numCol++;
    // headers << _( "URL");	_urlCol		= numCol++;

    setHeaderLabels( headers );
    header()->setSectionResizeMode( _nameCol, QHeaderView::Stretch );

    //setAllColumnsShowFocus( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );	// allow multi-selection with Ctrl-mouse

    connect( this, 	SIGNAL( itemSelectionChanged() ),
	     this, 	SLOT  ( filterIfVisible()) );
    setIconSize(QSize(32,32));
    fillList();
    setSortingEnabled( true );
    sortByColumn( nameCol(), Qt::AscendingOrder );
    selectSomething();

    // logVerbose() << "Creating repository list done" << endl;
}


YQPkgRepoList::~YQPkgRepoList()
{
    // NOP
}


void
YQPkgRepoList::fillList()
{
    clear();
    // logVerbose() << "Filling repository list" << endl;

    for ( ZyppRepositoryIterator it = ZyppRepositoriesBegin();
	  it != ZyppRepositoriesEnd();
	  ++it )
    {
	addRepo( *it );
    }

    // logVerbose() << "Inst repository filled" << endl;
}


int
YQPkgRepoList::countEnabledRepositories()
{
    return zyppPool().knownRepositoriesSize();
}


void
YQPkgRepoList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgRepoList::filter()
{
    emit filterStart();

#if TIME_FILTER
    logInfo() << "Collecting packages in selected repositories..." << endl;
    QElapsedTimer stopWatch;
    stopWatch.start();
#endif


    //
    // Collect all packages on this repository
    //

    QTreeWidgetItem * item;

    QList<QTreeWidgetItem *> items = selectedItems();
    QListIterator<QTreeWidgetItem *> it(items);

    while ( it.hasNext() )
    {
      item = it.next();
      YQPkgRepoListItem * repoItem = dynamic_cast<YQPkgRepoListItem *> (item);

        if ( repoItem )
        {
            ZyppRepo currentRepo = repoItem->zyppRepo();

	    zypp::PoolQuery query;
	    query.addRepo( currentRepo.info().alias() );
	    query.addKind(zypp::ResKind::package);

    	    for( zypp::PoolQuery::Selectable_iterator it = query.selectableBegin();
	         it != query.selectableEnd(); it++)
    	    {
		emit filterMatch( *it, tryCastToZyppPkg( (*it)->theObj() ) );
    	    }
	}
    }

#if TIME_FILTER
    logDebug() << "Packages sent to package list. Elapsed time: "
	       << stopWatch.elapsed() / 1000.0 << " sec"
	       << endl;
#endif

    emit filterFinished();
}


void
YQPkgRepoList::addRepo( ZyppRepo repo )
{
    new YQPkgRepoListItem( this, repo );
}


YQPkgRepoListItem *
YQPkgRepoList::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgRepoListItem *> (item);
}


YQPkgRepoListItem::YQPkgRepoListItem( YQPkgRepoList *	repoList,
				      ZyppRepo		repo	)
    : QY2ListViewItem( repoList )
    , _repoList( repoList )
    , _zyppRepo( repo )
{
    if ( nameCol() >= 0 )
    {
        string name = repo.info().name();
        if ( ! name.empty() )
        {
            setText( nameCol(), fromUTF8( name ));
        }
    }

    std::string infoToolTip;
    infoToolTip += ("<b>" + repo.info().name() + "</b>");

    ZyppProduct product = singleProduct( _zyppRepo );
    if ( product )
    {
        infoToolTip += ("<p>" + product->summary() + "</p>");
    }

    if ( ! repo.info().baseUrlsEmpty() )
    {
        zypp::RepoInfo::urls_const_iterator it;
        infoToolTip += "<ul>";

        for ( it = repo.info().baseUrlsBegin();
              it != repo.info().baseUrlsEnd();
              ++it )
        {
            infoToolTip += ("<li>" + (*it).asString() + "</li>");
        }
        infoToolTip += "</ul>";
     }
    setToolTip( nameCol(), fromUTF8(infoToolTip) );

    QString iconPath;
    QString iconName = "applications-internet";

    if ( ! repo.info().baseUrlsEmpty() )
    {
        zypp::Url repoUrl = *repo.info().baseUrlsBegin();

        if ( urlCol() >= 0 )
        {
            setText( urlCol(), repoUrl.asString().c_str() );
        }

        if (QString(repoUrl.asString().c_str()).contains("KDE") )
            iconName = "kde";
        if (QString(repoUrl.asString().c_str()).contains("GNOME") )
            iconName = "gnome";
        if (QString(repoUrl.asString().c_str()).contains("update") )
            iconName = "applications-utilities";
        if (QString(repoUrl.asString().c_str()).contains("home:") )
            iconName = "preferences-desktop";
    }

    if ( repo.isSystemRepo() )
        iconName = "preferences-system";

    setIcon( 0, QY2IconLoader::loadIcon( iconName ) );
}


YQPkgRepoListItem::~YQPkgRepoListItem()
{
    // NOP
}


ZyppProduct
YQPkgRepoListItem::singleProduct( ZyppRepo zyppRepo )
{
    return YQPkgFilters::singleProductFilter([&](const zypp::PoolItem& item) {
        // filter the products from the requested repository
        return item.resolvable()->repoInfo().alias() == zyppRepo.info().alias();
    });
}

bool
YQPkgRepoListItem::operator< ( const QTreeWidgetItem & other ) const
{
    const YQPkgRepoListItem * otherItem = dynamic_cast<const YQPkgRepoListItem *>(&other);

    return zyppRepo().info().name() < otherItem->zyppRepo().info().name();
}

