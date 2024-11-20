/*
 *   File name: YQPkgApplication.h
 *   Summary:	Application class for yqpkg
 *   License:	GPL V2 - See file LICENSE for details.
 *              Copyright (c) 2024 SUSE LLC
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>     // getuid()

#include <QApplication>

#include "YQi18n.h"
#include "YQPackageSelector.h"
#include "YQPkgApplication.h"
#include "Logger.h"
#include "Exception.h"


YQPkgApplication * YQPkgApplication::_instance = 0;


YQPkgApplication::YQPkgApplication()
    : QObject()
    , _pkgSel(0)
{
    _instance = this;
    logDebug() << "Creating YQPkgApplication" << endl;

    initZypp();
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;

    if ( _pkgSel )
    {
        delete _pkgSel;
        _pkgSel = 0;
    }

    shutdownZypp();

    logDebug() << "Destroying YQPkgApplication done" << endl;
    _instance = 0;
}


void YQPkgApplication::run()
{
    createPkgSel();
    qApp->exec();
}


void YQPkgApplication::createPkgSel()
{
    if ( _pkgSel )
        return;

    _pkgSel = new YQPackageSelector( 0, 0 );
    CHECK_PTR( _pkgSel );

    QObject::connect( _pkgSel, SIGNAL( commit() ),
                      qApp,    SLOT  ( quit()   ) );

    _pkgSel->show();
}


bool YQPkgApplication::runningAsRoot()
{
    return geteuid() == 0;
}


void YQPkgApplication::initZypp()
{
    logDebug() << "Initializing zypp..." << endl;
    zypp_ptr()->initializeTarget( "/",
                                  false );      // rebuild_rpmdb
    zypp_ptr()->target()->load();

    logDebug() << "Initializing zypp done" << endl;
}


void YQPkgApplication::shutdownZypp()
{
    logDebug() << "Shutting down zypp..." << endl;

    _zypp_pointer = 0;

    logDebug() << "Shutting down zypp done" << endl;
}

//
// Stolen from yast-pkg-bindings/src/PkgFunctions.cc
//

zypp::ZYpp::Ptr
YQPkgApplication::zypp_ptr()
{
    if ( _zypp_pointer )
	return _zypp_pointer;

    int max_count = 5;
    unsigned int wait_seconds = 3;

    while ( _zypp_pointer == NULL && max_count > 0 )
    {
	try
	{
	    logInfo() << "Initializing Zypp library..." << endl;
	    _zypp_pointer = zypp::getZYpp();

 	    // initialize solver flag, be compatible with zypper
	    _zypp_pointer->resolver()->setIgnoreAlreadyRecommended( true );

	    return _zypp_pointer;
	}
	catch ( const zypp::Exception & ex )
	{
	    if ( max_count == 1 )  // last attempt?
		ZYPP_RETHROW( ex );
	}

	max_count--;

	if ( _zypp_pointer == NULL && max_count > 0 )
	    sleep( wait_seconds );
    }

    if ( _zypp_pointer == NULL )
    {
	// Still not initialized; throw an exception.
	// Translators: This is an error message
	THROW( Exception( _( "Cannot connect to the package manager" ) ) );
    }

    return _zypp_pointer;
}

