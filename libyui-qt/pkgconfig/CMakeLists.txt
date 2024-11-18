# CMakeLists.txt for libyui-qt/pkgconfig
#
# Support for pkg-config:
#
# Generate a libyui-qt.pc file from libyui-qt.pc.in and install it to
# /usr/lib64/pkgconfig.
#
# A .pc file specifies how to use a development package, in particular linker
# flags (-lyui), compiler flags (including include directories) and paths.
# See man pkg-config.
#
# This .pc file is intended for extensions of the Qt UI plug-in, not for
# applications using libyui.

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_LIBDIR

# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
# CMAKE_INSTALL_FULL_<dir>
# The absolute path generated from the corresponding CMAKE_INSTALL_<dir> value.
# If the value is not already an absolute path, an absolute path is constructed
# typically by prepending the value of the CMAKE_INSTALL_PREFIX variable.
# So that covers also cmake directive -DCMAKE_INSTALL_LIBDIR=/usr/lib64 with full
# pathname.
# CMAKE_INSTALL_FULL_LIBDIR is used to install libyui-qt.pc and inside it
# to set "libdir" and "plugindir" pkg-config variables
set( PKGCONFIG_INSTALL_DIR ${DESTDIR}${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig )

# Generate libyui-qt.pc where some CMake variables are expanded from libyui-qt.pc.in,
# but only expand @VARIABLE@, not ${VARIABLE}
configure_file( libyui-qt.pc.in libyui-qt.pc @ONLY )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libyui-qt.pc DESTINATION ${PKGCONFIG_INSTALL_DIR} )
