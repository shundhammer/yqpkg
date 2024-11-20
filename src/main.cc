/*
 *   File name: main.cpp
 *   Summary:	yqpkg main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <shundhammer@suse.com>
 */


//  #include <iostream>	// cerr

#include <QApplication>
#include <QObject>
#include "YQPackageSelector.h"
#include "Logger.h"
#include "Exception.h"


// using std::cerr;
static const char * progName = "yqpkg";


void logVersion()
{
    // VERSION is imported from the toplevel VERSION.cmake file
    // via a compiler command line "-DVERSION=..."

    logInfo() << progName << "-" << VERSION
              << " built with Qt " << QT_VERSION_STR
              << endl;
}


int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/yqpkg-$USER", "yqpkg.log" );
    logVersion();

    // Set org/app name for QSettings
    QCoreApplication::setOrganizationName( "openSUSE" );
    QCoreApplication::setApplicationName ( progName );

    QApplication qtApp( argc, argv);
    QStringList argList = QCoreApplication::arguments();
    argList.removeFirst(); // Remove program name

    QWidget * mainWin = new YQPackageSelector( 0, 0 );
    CHECK_PTR( mainWin );
    mainWin->show();

    QObject::connect( mainWin, SIGNAL( commit() ),
                      &qtApp,  SLOT  ( quit()   ) );

    qtApp.exec();

    delete mainWin;

    return 0;
}
