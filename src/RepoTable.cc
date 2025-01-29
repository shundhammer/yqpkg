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

}




RepoTableItem::RepoTableItem( RepoTable *    parentTable,
                              ZyppRepoInfo * repoInfo    )
    : QY2ListViewItem( parentTable )
    , _parentTable( parentTable )
    , _repoInfo( repoInfo )
{

}


RepoTableItem::~RepoTableItem()
{

}


bool
RepoTableItem::operator< ( const QTreeWidgetItem & other ) const
{
    return QY2ListViewItem::operator<( other );
}
