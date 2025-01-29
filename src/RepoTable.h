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


#ifndef RepoTable_h
#define RepoTable_h

#include <string>

#include "YQZypp.h"
#include "QY2ListView.h"


namespace zypp
{
    class RepoManager;
};


/**
 * Multi-column table widget to show libzypp repositories.
 *
 * This is the table used in the RepoEditor, not to confuse with the simple
 * one-column YQPkgRepoList used in the YQPkgRepoFilterView.
 **/
class RepoTable: public QY2ListView
{
    Q_OBJECT

public:

    /**
     * Table columns
     **/
    enum Col
    {
        NameCol,
        PrioCol,
        EnabledCol,
        AutoRefCol,
        ServiceCol,
        UrlCol
    };


    /**
     * Constructor
     **/
    RepoTable( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~RepoTable();

    /**
     * Populate with the repos from the specified repo manager.
     **/
    void populate( zypp::RepoManager * repoManager );
};


class RepoTableItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    RepoTableItem( RepoTable *    parentTable = 0,
                   ZyppRepoInfo * repoInfo    = 0 );

    /**
     * Destructor
     **/
    virtual ~RepoTableItem();

    /**
     * Return the associated RepoInfo.
     **/
    ZyppRepoInfo * repoInfo() const { return _repoInfo; }

    /**
     * Comparison function used for sorting the list.
     * Reimplemented from QY2ListViewItem / QTreeWidgetItem.
     **/
    virtual bool operator< ( const QTreeWidgetItem & other ) const override;

    /**
     * Update the data for this item.
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void updateData() override;

    /**
     * Set a column text
     **/
    void setText( int col, const std::string & txt );
    void setText( int col, const QString & txt );


protected:

    /**
     * Return a "checkmark" icon.
     **/
    QPixmap checkmarkIcon();

    /**
     * Return an empty icon.
     **/
    QPixmap noIcon();


    // Data members

    RepoTable *    _parentTable;
    ZyppRepoInfo * _repoInfo;
};


#endif // RepoTable_h
