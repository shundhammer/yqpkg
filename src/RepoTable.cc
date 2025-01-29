/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */



#include <zypp/RepoManager.h>

#include "Exception.h"
#include "Logger.h"
#include "YQIconPool.h"
#include "utf8.h"
#include "RepoTable.h"


RepoTable::RepoTable( QWidget * parent )
    : QY2ListView( parent )
{

}


RepoTable::~RepoTable()
{
    // NOP
}


void RepoTable::populate( zypp::RepoManager * repoManager )
{
    for ( zypp::RepoManager::RepoConstIterator it = repoManager->repoBegin();
          it != repoManager->repoEnd();
          ++it )
    {
        ZyppRepoInfo repoInfo = *it;

        RepoTableItem * item = new RepoTableItem( this, &repoInfo );
        CHECK_NEW( item );
    }
}




RepoTableItem::RepoTableItem( RepoTable *    parentTable,
                              ZyppRepoInfo * repoInfo    )
    : QY2ListViewItem( parentTable )
    , _parentTable( parentTable )
    , _repoInfo( repoInfo )
{
    updateData();
}


RepoTableItem::~RepoTableItem()
{
    // NOP
}


void RepoTableItem::updateData()
{
    setText( RepoTable::NameCol,    _repoInfo->name() );
    setIcon( RepoTable::EnabledCol, _repoInfo->enabled()     ? checkmarkIcon() : noIcon() );
    setIcon( RepoTable::AutoRefCol, _repoInfo->autorefresh() ? checkmarkIcon() : noIcon() );
    setText( RepoTable::PrioCol,    std::to_string( _repoInfo->priority() ) );
    setText( RepoTable::ServiceCol, _repoInfo->service() );
    setText( RepoTable::UrlCol,     _repoInfo->url().asString() );
}


void RepoTableItem::setText( int col, const std::string & txt )
{
    setText( col, fromUTF8( txt ) );
}


void RepoTableItem::setText( int col, const QString & txt )
{
    // This really shouldn't be necessary, but gcc already cost me a full hour
    // of lifetime and screaming at the screen and half a week of life energy
    // with this bullshit: It just wouldn't let me use the publicly inherited
    // method directly. So let's call the inherited method explicitly. WTF?!

    QY2ListViewItem::setText( col, txt );
}


QPixmap
RepoTableItem::checkmarkIcon()
{
    return YQIconPool::checkmark();
}


QPixmap
RepoTableItem::noIcon()
{
    return QPixmap();
}


bool
RepoTableItem::operator< ( const QTreeWidgetItem & otherTreeWidgetItem ) const
{
    int sortColumn = _parentTable ? _parentTable->sortColumn() : 0;

    Q_UNUSED( sortColumn );

    return QY2ListViewItem::operator<( otherTreeWidgetItem );
}
