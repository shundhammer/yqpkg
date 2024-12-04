# -*- mode: makefile -*-
#
# CMakeLists.txt for libyui-qt-pkg/src

include( ../VERSION.cmake )
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
  # BusyPopup.cc
  Logger.cc
  Exception.cc
  FSize.cc
  MainWindow.cc
  PkgCommitPage.cc
  SummaryPage.cc
  WindowSettings.cc
  Workflow.cc

  YQPackageSelector.cc
  YQPackageSelectorBase.cc
  YQPackageSelectorHelp.cc

  YQPkgApplication.cc
  YQPkgAppWorkflowSteps.cc
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
  YQPkgRepoManager.cc
  YQPkgSearchFilterView.cc
  YQPkgSecondaryFilterView.cc
  YQPkgSelDescriptionView.cc
  YQPkgSelMapper.cc
  YQPkgServiceFilterView.cc
  YQPkgServiceList.cc
  YQPkgStatusFilterView.cc
  YQPkgTechnicalDetailsView.cc
  YQPkgTextDialog.cc
  YQPkgVersionsView.cc

  YQIconPool.cc
  YQSignalBlocker.cc
  QY2ComboTabWidget.cc
  QY2CursorHelper.cc
  QY2DiskUsageList.cc
  QY2IconLoader.cc
  QY2LayoutUtils.cc
  QY2ListView.cc
  )

# Qt resource files
set( QRC_FILES icons.qrc ) # Compiled-in resources: icons

#
# .ui widget forms generated with Qt designer (designer-qt5) or Qt Creator
#
# For each one, the Qt uic (User Interface Compiler) generates a file
# ui_formname.h at build time to include in a .cc file.
# This is done automatically with CMAKE_AUTOUIC above.
#
# See WizardPage.{h,cc} and wizard-page.ui (and the generated ui_wizard-page.h)
# for a very simple usage example.
#

set( UI_FILES
     pkg-commit-page.ui
     summary-page.ui
   )

# Putting it all together
add_executable( ${TARGETBIN}
  ${SOURCES}
  ${QRC_FILES}
  ${UI_FILES}
)


#
# Compile options and definitions
#

# Fix things that were introduced with Qt 5.15 for Qt 6.x compatibility,
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


# Libraries that are needed to build this executable
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( yqpkg
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

install(TARGETS ${TARGETBIN} RUNTIME DESTINATION bin)
