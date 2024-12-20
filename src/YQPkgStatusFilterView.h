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


#ifndef YQPkgStatusFilterView_h
#define YQPkgStatusFilterView_h

#include "YQZypp.h"
#include <QWidget>
#include <QPixmap>
#include <QScrollArea>


class QComboBox;
class QCheckBox;
class QPushButton;


/**
 * Filter view for packages by status
 **/
class YQPkgStatusFilterView : public QScrollArea
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgStatusFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgStatusFilterView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Check if pkg matches the filter criteria.
     **/
    bool check( ZyppSel selectable,
                ZyppObj pkg );


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


    /**
     * Reset all check boxes (set them all to "off")
     **/
    void clear();

    /**
     * Set up the check boxes so all pending transactions are displayed.
     **/
    void showTransactions();

    /**
     * Read settings from the config file.
     **/
    void readSettings();

    /**
     * Write settings to the config file.
     **/
    void writeSettings();


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel selectable,
                      ZyppPkg pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected:

    /**
     * Add a check box
     **/
    QCheckBox * addStatusCheckBox( QWidget *       parent,
                                   const QString & label,
                                   const QPixmap & icon,
                                   bool            initiallyChecked );

    // Data members

    QCheckBox *    _showAutoDel;
    QCheckBox *    _showAutoInstall;
    QCheckBox *    _showAutoUpdate;
    QCheckBox *    _showDel;
    QCheckBox *    _showInstall;
    QCheckBox *    _showKeepInstalled;
    QCheckBox *    _showNoInst;
    QCheckBox *    _showTaboo;
    QCheckBox *    _showProtected;
    QCheckBox *    _showUpdate;

    QPushButton * _refreshButton;
};



#endif // ifndef YQPkgStatusFilterView_h
