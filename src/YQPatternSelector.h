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


#ifndef YQPatternSelector_h
#define YQPatternSelector_h

#include <QWidget>
#include <QColor>

#include "YQPackageSelectorBase.h"


class QPushButton;

class YQPkgSelDescriptionView;
class YQPkgPatternList;


class YQPatternSelector: public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQPatternSelector( QWidget * parent, long modeFlags );


protected slots:

    /**
     * User clicked on "Details..." - start the detailed package selection.
     * This will return from UI::RunPkgSelection() with :details .
     **/
    void detailedPackageSelection();

    /**
     * Debugging
     **/
    void debugTrace();


protected:

    // Layout methods - create and layout widgets

    void	basicLayout();
    QWidget *	layoutLeftPane	( QWidget * parent );
    QWidget *	layoutRightPane	( QWidget * parent );
    void	layoutButtons	( QWidget * parent );

    /**
     * Establish Qt signal / slot connections.
     **/
    void makeConnections();


    // Data members

    YQPkgPatternList *		_patternList;
    YQPkgSelDescriptionView *	_descriptionView;
};



#endif // YQPatternSelector_h
