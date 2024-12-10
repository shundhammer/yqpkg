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


#include "Logger.h"
#include "Exception.h"
#include "PkgCommitCallbacks.h"


PkgCommitCallbacks::PkgCommitCallbacks()
{
    _pkgDownloadCallback.connect();
    _pkgInstallCallback.connect();
    _pkgRemoveCallback.connect();
}

PkgCommitCallbacks::~PkgCommitCallbacks()
{
    // Each zypp::Callback disconnects automatically in the dtor.
}


//
//----------------------------------------------------------------------
//


PkgCommitSignalForwarder * PkgCommitSignalForwarder::_instance = 0;


PkgCommitSignalForwarder * PkgCommitSignalForwarder::instance()
{
    if ( ! _instance )
    {
        _instance = new PkgCommitSignalForwarder();
        CHECK_NEW( _instance );
    }

    return _instance;
}


void PkgCommitSignalForwarder::connectAll( QObject * receiver )
{
    QObject::connect( instance(), SIGNAL( pkgDownloadStart()    ),
                      receiver,   SLOT  ( pkgDownloadStart()    ) );

    QObject::connect( instance(), SIGNAL( pkgDownloadProgress() ),
                      receiver,   SLOT  ( pkgDownloadProgress() ) );

    QObject::connect( instance(), SIGNAL( pkgDownloadEnd()      ),
                      receiver,   SLOT  ( pkgDownloadEnd()      ) );


    QObject::connect( instance(), SIGNAL( pkgInstallStart()     ),
                      receiver,   SLOT  ( pkgInstallStart()     ) );

    QObject::connect( instance(), SIGNAL( pkgInstallProgress()  ),
                      receiver,   SLOT  ( pkgInstallProgress()  ) );

    QObject::connect( instance(), SIGNAL( pkgInstallEnd()       ),
                      receiver,   SLOT  ( pkgInstallEnd()       ) );


    QObject::connect( instance(), SIGNAL( pkgRemoveStart()      ),
                      receiver,   SLOT  ( pkgRemoveStart()      ) );

    QObject::connect( instance(), SIGNAL( pkgRemoveProgress()   ),
                      receiver,   SLOT  ( pkgRemoveProgress()   ) );

    QObject::connect( instance(), SIGNAL( pkgRemoveEnd()        ),
                      receiver,   SLOT  ( pkgRemoveEnd()        ) );
}
