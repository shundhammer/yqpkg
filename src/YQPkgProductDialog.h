/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef YQPkgProductDialog_h
#define YQPkgProductDialog_h

#include <QDialog>


class QTabWidget;
class YQPkgProductList;
class YQPkgDependenciesView;


/**
 * Products list with dependencies details view
 **/
class YQPkgProductDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a dialog with all products
     **/
    static void showProductDialog( QWidget* parent = 0);

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint() const;

    /**
     * Delayed initialization after the dialog is fully created.
     *
     * Reimplemented from QWidget.
     **/
    virtual void polish();


protected:

    /**
     * Constructor.
     **/
    YQPkgProductDialog( QWidget * parent );


    // Data members

    YQPkgProductList *      _productList;
    QTabWidget *            _detailsViews;
    YQPkgDependenciesView * _dependenciesView;
};


#endif // ifndef YQPkgProductDialog_h
