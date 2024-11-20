/*
 *   File name: YQPkgApplication.h
 *   Summary:	Application class for yqpkg
 *   License:	GPL V2 - See file LICENSE for details.
 *              Copyright (c) 2024 SUSE LLC
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>

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
}


YQPkgApplication::~YQPkgApplication()
{
    logDebug() << "Destroying YQPkgApplication..." << endl;
    
    if ( _pkgSel )
    {
        delete _pkgSel;
        _pkgSel = 0;
    }
    
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
