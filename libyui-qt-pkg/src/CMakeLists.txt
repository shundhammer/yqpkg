# -*- mode: makefile -*-
#
# CMakeLists.txt for libyui-qt-pkg/src

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

#
# Qt-specific
#
# See also
#  https://doc.qt.io/qt-5/cmake-get-started.html
#

set( TARGETBIN yqpkg )

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )
# find_library( zypp ) is pointless because there is a libzypp on every SUSE

set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)
set( CMAKE_AUTORCC on ) # Automatically handle Qt resource (.rcc) files
set( CMAKE_AUTOUIC on ) # Automatically handle Qt Designer (.uic) files


set( SOURCES

  main.cc
  Logger.cc
  Exception.cc

  YQPackageSelector.cc
  YQPackageSelectorBase.cc
  YQPackageSelectorHelp.cc
  YQPatternSelector.cc
  YQSimplePatchSelector.cc

  YQPkgChangeLogView.cc
  YQPkgChangesDialog.cc
  YQPkgClassFilterView.cc
  YQPkgConflictDialog.cc
  YQPkgConflictList.cc
  YQPkgDependenciesView.cc
  YQPkgDescriptionDialog.cc
  YQPkgDescriptionView.cc
  YQPkgDiskUsageList.cc
  YQPkgDiskUsageWarningDialog.cc
  YQPkgFileListView.cc
  YQPkgFilterTab.cc
  YQPkgFilters.cc
  YQPkgGenericDetailsView.cc
  YQPkgHistoryDialog.cc
  YQPkgLangList.cc
  YQPkgList.cc
  YQPkgObjList.cc
  YQPkgPatchFilterView.cc
  YQPkgPatchList.cc
  YQPkgPatternList.cc
  YQPkgProductDialog.cc
  YQPkgProductList.cc
  YQPkgRepoFilterView.cc
  YQPkgRepoList.cc
  YQPkgSearchFilterView.cc
  YQPkgSecondaryFilterView.cc
  YQPkgSelDescriptionView.cc
  YQPkgSelMapper.cc
  YQPkgServiceFilterView.cc
  YQPkgServiceList.cc
  YQPkgStatusFilterView.cc
  YQPkgTechnicalDetailsView.cc
  YQPkgTextDialog.cc
  YQPkgUpdateProblemFilterView.cc
  YQPkgVersionsView.cc

  YQIconPool.cc
  YQSignalBlocker.cc
  QY2ComboTabWidget.cc
  QY2CursorHelper.cc
  QY2DiskUsageList.cc
  QY2LayoutUtils.cc
  QY2ListView.cc
  )

set( QRC_FILES qt_pkg_icons.qrc ) # Compiled-in resources: icons

add_executable( ${TARGETBIN}
  ${SOURCES}
  ${QRC_FILES}
)


#
# Include directories and compile options
#

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Create an include dir with some symlinks to include headers from sibling projects:
# build/src/include/yui/qt  -> ../../libyui-qt/src
# build/src/include/yui/yui -> ../../libyui/src
file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR}/yui )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src    ${LOCAL_INCLUDE_DIR}/yui/yui SYMBOLIC )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui-qt/src ${LOCAL_INCLUDE_DIR}/yui/qt  SYMBOLIC )

# <yui/qt/YQFoo.h> from build/src/include/yui/qt -> ../../libyui-qt/src
target_include_directories( ${TARGETBIN} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )

# <yui/YFoo.h> from build/src/include/yui/yui -> ../../libyui/src
target_include_directories( ${TARGETBIN} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR}/yui )


#
# Compile options and definitions
#

# Fix things that were introduce with Qt 5.15 for Qt 6.x compatibility,
# but that completely disregard that this introduces incompatibilities
# with older code.

target_compile_options( ${TARGETBIN} PUBLIC "-Wno-deprecated" )
target_compile_options( ${TARGETBIN} PUBLIC "-Wno-deprecated-declarations" )

# Make the version from ../../VERSION.cmake available as a #define
target_compile_definitions( ${TARGETBIN} PUBLIC VERSION="${VERSION}" )

# Workaround for boost::bind() complaining about deprecated _1 placeholder
# deep in the libzypp headers
target_compile_definitions( ${TARGETBIN} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS=1 )

# Add more compile options to this target in addition to those
# added in the toplevel CMakeLists.txt.
#
# Notice that CMake will automatically add -fPIC etc. where needed,
# like for this shared lib.
### target_compile_options( ${TARGETBIN} PUBLIC "-Dfoo" )

# Show the complete compiler commands with all arguments:
#   make VERBOSE=1

# Add more compile options to an individual source file:
### set_source_files_properties( YUI.cc PROPERTIES COMPILE_OPTIONS "-Dfoo" )




#
# Linking
#

# Find yui during a combined build
target_link_directories( ${TARGETBIN} BEFORE PUBLIC ../../libyui/build/src )


# Libraries that are needed to build this shared lib
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( yqpkg
  yui
  zypp
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  )

# Notice that we don't link against Qt5::Svg, but we need it at runtime:
#
# It's a plugin for Qt and will be used to load SVGs (like our icons) if
# libQt5Svg is available. But we don't use it directly here, only via Qt
# classes like QPixmap and QIcon. Qt takes loads the SVG plugin as needed.



#
# Install
#

## FIXME install the binary to BINDIR (or whatever)

##  install( TARGETS ${TARGETBIN} LIBRARY DESTINATION ${PLUGIN_DIR} )
