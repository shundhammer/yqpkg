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


#ifndef YQPkgPatchFilterView_h
#define YQPkgPatchFilterView_h

#include "YQZypp.h"
#include "YQPkgSelMapper.h"
#include <QLabel>


class YQPkgPatchList;
class YQPkgDescriptionView;
class QComboBox;
class QLabel;
class QSplitter;
class QTabWidget;


/**
 * Display a list of zypp::Patch objects and details about the currently
 * selected patch.
 **/
class YQPkgPatchFilterView : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgPatchFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchFilterView();


    /**
     * Returns this view's selections list.
     **/
    YQPkgPatchList * patchList() const { return _patchList; }


public slots:

    /**
     * Update the "total download size" field.
     **/
    void updateTotalDownloadSize();


protected slots:

    /**
     * Fill the patch list with regard to the _patchCategory
     * combo box.
     **/
    void fillPatchList();


protected:


    // Data members

    QSplitter *            _splitter;
    YQPkgPatchList *       _patchList;
    QComboBox *            _patchFilter;
    QTabWidget *           _detailsViews;
    YQPkgDescriptionView * _descriptionView;
    QLabel *               _totalDownloadSize;

    YQPkgSelMapper         _selMapper;
};



#endif // ifndef YQPkgPatchFilterView_h
