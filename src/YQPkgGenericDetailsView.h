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


#ifndef YQPkgGenericDetailsView_h
#define YQPkgGenericDetailsView_h

#include <QTextBrowser>
#include <zypp/Date.h>
#include "YQZypp.h"


class QTabWidget;
using std::string;


/**
 * @short Abstract base class for details views. Handles generic stuff like
 * HTML formatting, Qt slots and display only if this view is visible at all
 * ( it may be hidden if it's part of a QTabWidget ).
 **/
class YQPkgGenericDetailsView : public QTextBrowser
{
    Q_OBJECT

protected:

    /**
     * Constructor.
     **/
    YQPkgGenericDetailsView( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgGenericDetailsView();


public:

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * starts the html tag and set the style
     */
    static QString htmlStart();
    static QString htmlEnd();

    /**
     * Returns a uniform heading in HTML format for the specified selectable:
     * name and summary
     * or name, version and summary, if 'showVersion' is 'true'
     **/
    static QString htmlHeading( ZyppSel selectable, bool showVersion = false );

    /**
     * Escapes characters special to HTML in a ( plain text ) string, such as:
     * '<'   ->   '&lt;'
     * '>'   ->   '&gt;'
     * '&'   ->   '&amp;'
     *
     * Returns the escaped string.
     **/
    static QString htmlEscape( const QString & plainText );

    /**
     * Returns a string containing a HTML table with 'contents'.
     **/
    static QString table( const QString & contents );

    /**
     * Returns a string containing a HTML table row with 'contents'.
     **/
    static QString row( const QString & contents );

    /**
     * Returns a string containing a HTML table cell with 'contents'.
     **/
    static QString cell( QString contents		);
    static QString cell( int contents 			);
    static QString cell( const string & contents	);
    static QString cell( const zypp::Date & date	);

    /**
     * Returns a string containing a HTML table cell with 'contents'
     * for table headers.
     **/
    static QString hcell( QString contents );


public slots:

    /**
     * Show details for the specified package.
     * Delayed ( optimized ) display if this is embedded into a QTabWidget
     * parent: In this case, wait until this page becomes visible.
     **/
    void showDetailsIfVisible( ZyppSel selectable );

    // slot clear() inherited from QTextEdit

    /**
     * Show details for the specified package.
     * Reimplement this in derived classes.
     **/
    virtual void showDetails( ZyppSel selectable ) = 0;


    
protected slots:

    /**
     * Show data for the last package.
     **/
    void reloadTab( int newCurrent );
    virtual void reload() { QTextBrowser::reload(); }


protected:
    
    // Data members

    QTabWidget	* 	_parentTab;
    ZyppSel		_selectable;
};


#endif // ifndef YQPkgGenericDetailsView_h
