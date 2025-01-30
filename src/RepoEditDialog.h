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


#ifndef RepoEditDialog_h
#define RepoEditDialog_h


#include <QDialog>
#include "YQZypp.h"     // ZyppRepoInfo


// Generated with 'uic' from a Qt designer .ui form: repo-edit-add.ui
//
// Check out ../build/src/myrlyn_autogen/include/ui_repo-edit-add.h
// for the variable names of the widgets.

#include "ui_repo-edit-add.h"


/**
 * Dialog class to edit and add a repo.
 * This includes adding a community repo like Packman.
 **/
class RepoEditDialog: public QDialog
{
    Q_OBJECT

public:

    enum Mode
    {
        AddRepo,
        EditRepo
    };


    /**
     * Constructor.
     *
     * 'mode' is important here mostly to make the repoTypeContainer visible
     * so the user can also choose a predefined community repo.
     **/
    RepoEditDialog( Mode      mode   = AddRepo,
                    QWidget * parent = 0 );

    /**
     * Destructor.
     **/
    virtual ~RepoEditDialog();

    /**
     * Add a repo: Show and execute the dialog until the user hits either the
     * "OK" or the "Cancel" button. This returns QDialog::Accepted if the
     * dialog was ended with "OK" or QDialog::Rejected if it was ended with
     * "Cancel".
     *
     * For QDialog::Accepted, use repoInfo() to get the information for the
     * repo and use the repo manager to actually add it.
     **/
    int addRepo();

    /**
     * Edit a repo: Show and execute the dialog until the user hits either the
     * "OK" or the "Cancel" button. This returns QDialog::Accepted if the
     * dialog was ended with "OK" or QDialog::Rejected if it was ended with
     * "Cancel".
     *
     * For QDialog::Accepted, use repoInfo() to get the information for the
     * repo and use the repo manager to actually modify it.
     **/
    int editRepo( const ZyppRepoInfo & repoInfo );

    /**
     * Return the information for the added or edited repo if the user if the
     * user closed the dialog with "OK".
     **/
    ZyppRepoInfo repoInfo() const { return _repoInfo; }


protected slots:

    /**
     * Slot to enable or disable the community repos list when the user hit one
     * of the "Custom Repo" or "Community Repo" radio buttons.
     **/
    void repoTypeChanged();

    /**
     * Update the expandedUrl label below the repoRawUrl lineedit with the URL
     * with variables like $releasever expanded.
     **/
    void updateExpandedUrl();


protected:

    /**
     * Set up signal / slot connections between the widgets.
     **/
    void connectWidgets();

    /**
     * Get the current values out of the widgets and save them in _repoInfo.
     **/
    void saveRepoInfo();


    //
    // Data members
    //

    Mode           _mode;
    Ui::RepoEdit * _ui;
    ZyppRepoInfo   _repoInfo;

    QString        _oldRepoName;
    QString        _oldRawUrl;
};

#endif // RepoEditDialog_h
