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


#ifndef YQPkgSearchFilterView_h
#define YQPkgSearchFilterView_h

#include "YQZypp.h"
#include <QWidget>
#include <QRegExp>
#include <QEvent>
#include <QScrollArea>

#include "SearchFilter.h"


class QComboBox;
class QCheckBox;
class QPushButton;
class QRadioButton;

using std::string;


/**
 * Filter view for searching within packages
 **/
class YQPkgSearchFilterView : public QScrollArea
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSearchFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSearchFilterView();

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Check one ResObject against the currently selected values.
     * Returns true if the package matches, false if not.
     **/
    bool check( ZyppSel selectable,
                ZyppObj zyppObj );


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
     * Set the keyboard focus into this view's input field.
     **/
    void setFocus();


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

    /**
     * Send a short message about unsuccessful searches.
     **/
    void message( const QString & text );


protected:

    /**
     * Build a SearchFilter object from the widgets.
     **/
    SearchFilter buildSearchFilterFromWidgets();

    /**
     * Key press event: Execute search upon 'Return'
     * Reimplemented from QVBox / QWidget.
     **/
    virtual void keyPressEvent( QKeyEvent * event );

    /**
     * Read settings from the config file.
     **/
    void readSettings();

    /**
     * Write settings to the config file.
     **/
    void writeSettings();


    //
    // Data members
    //

    QComboBox *   _searchText;
    QPushButton * _searchButton;

    QCheckBox *   _searchInName;
    QCheckBox *   _searchInSummary;
    QCheckBox *   _searchInDescription;
    QCheckBox *   _searchInRequires;
    QCheckBox *   _searchInProvides;
    QCheckBox *   _searchInFileList;

    QComboBox *   _searchMode;
    QCheckBox *   _caseSensitive;

    int           _matchCount;
};


#endif // ifndef YQPkgSearchFilterView_h
