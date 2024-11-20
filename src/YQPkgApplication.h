/*
 *   File name: YQPkgApplication.h
 *   Summary:	Application class for yqpkg
 *   License:	GPL V2 - See file LICENSE for details.
 *              Copyright (c) 2024 SUSE LLC
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef YQPkgApplication_h
#define YQPkgApplication_h

#include <QObject>


class YQPackageSelector;

/**
 * Application class for yqpkg.
 **/
class YQPkgApplication: public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     **/
    YQPkgApplication();

    /**
     * Destructor
     **/
    virtual ~YQPkgApplication();

    /**
     * Return the instance of this class or 0 if there is none.
     *
     * This is not a real singleton, but for the life time of this application
     * this instance will remain alive, i.e. for most other classes related to
     * this.
     **/
    static YQPkgApplication * instance() { return _instance; }

    /**
     * Run the application. This also handles the Qt event loop.
     **/
    void run();

    /**
     * Return 'true' if this program is running with root privileges.
     **/
    static bool runningAsRoot();

protected:

    /**
     * Create (and show) the YQPackageSelector if it doesn't already exist.
     **/
    void createPkgSel();


    //
    // Data members
    //

    YQPackageSelector * _pkgSel;
    
    static YQPkgApplication * _instance;
};

#endif // YQPkgApplication_h
