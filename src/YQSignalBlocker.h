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


#ifndef YQSignalBlocker_h
#define YQSignalBlocker_h

class QObject;

/**
 * Helper class to block Qt signals for QWidgets or QObjects as long as this
 * object exists.
 *
 * This object will restore the old blocked state when it goes out of scope.
 **/
class YQSignalBlocker
{
public:

    /**
     * Constructor.
     *
     * Qt signals for 'qobject' will be blocked as long as this object exists.
     * Remember that a QWidget is also a QObject.
     **/
    YQSignalBlocker( QObject * qobject );

    /**
     * Destructor.
     *
     * This will restore the old signal state.
     **/
    ~YQSignalBlocker();


private:

    QObject *	_qobject;
    bool	_oldBlockedState;
};


#endif // ifndef YQSignalBlocker_h
