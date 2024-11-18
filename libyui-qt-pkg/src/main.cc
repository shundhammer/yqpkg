/*
 *   File name: main.cpp
 *   Summary:	yqpkg main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <shundhammer@suse.com>
 */


#include <iostream>	// cerr

#include <QApplication>
#include "Logger.h"
#include "Version.h"


using std::cerr;
static const char * progName = "yqpkg";


void logVersion()
{
    logInfo() << "yqpkg" << YQPKG_VERSION
              << " built with Qt " << QT_VERSION_STR
              << endl;
}


int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/yqpkg-$USER", "yqpkg.log" );
    logVersion();

    // Set org/app name for QSettings
    QCoreApplication::setOrganizationName( "openSUSE" );
    QCoreApplication::setApplicationName ( "yqpkg" );

    QApplication qtApp( argc, argv);
    QStringList argList = QCoreApplication::arguments();
    argList.removeFirst(); // Remove program name

    MainWindow * mainWin = new YQPackageSelector();
    CHECK_PTR( mainWin );
    mainWin->show();

    qtApp.exec();

    delete mainWin;

    return fatal ? 1 : 0;
}
