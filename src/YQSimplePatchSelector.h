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


#ifndef YQSimplePatchSelector_h
#define YQSimplePatchSelector_h

#include <QColor>

#include "YQPackageSelectorBase.h"


class QPushButton;

class YQPkgPatchFilterView;
class YQPkgPatchList;


class YQSimplePatchSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQSimplePatchSelector( YWidget * parent, long modeFlags );


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
    void	layoutButtons	( QWidget * parent );

    /**
     * Establish Qt signal / slot connections.
     **/
    void makeConnections();


    // Data members

    YQPkgPatchFilterView *	_patchFilterView;
    YQPkgPatchList *		_patchList;
};



#endif // YQSimplePatchSelector_h
