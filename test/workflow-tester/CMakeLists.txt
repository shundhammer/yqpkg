# -*- mode: makefile -*-
#
# CMakeLists.txt for yqpkg/test/workflow-tester
#
# Building:
#
#   cd <project-root>
#   mkdir build
#   cd build
#   cmake -DBUILD_TEST=on -DBUILD_SRC=on ..
#   make
#
# or
#
#   cd test
#   make
#
# Start with
#
#   test/workflow-tester/workflow-tester

include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

#
# Qt-specific
#
# See also
#  https://doc.qt.io/qt-5/cmake-get-started.html
#

set( TARGETBIN workflow-tester )

find_package( Qt5 COMPONENTS Core REQUIRED )
# find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)
set( CMAKE_AUTORCC on ) # Automatically handle Qt resource (.rcc) files
set( CMAKE_AUTOUIC on ) # Automatically handle Qt Designer (.uic) files


set( SOURCES
  workflow-tester.cc
  ../../src/Logger.cc
  ../../src/Exception.cc
  ../../src/Workflow.cc
  )

# set( UI_FILES workflow-tester.ui )

add_executable( ${TARGETBIN}
  ${SOURCES}
#  ${UI_FILES}
)


#
# Compile options and definitions
#

# Fix things that were introduced with Qt 5.15 for Qt 6.x compatibility,
# but that completely disregard that this introduces incompatibilities
# with older code.

target_compile_options( ${TARGETBIN} PUBLIC "-Wno-deprecated" )
target_compile_options( ${TARGETBIN} PUBLIC "-Wno-deprecated-declarations" )

# Workaround for boost::bind() complaining about deprecated _1 placeholder
# deep in the libzypp headers
target_compile_definitions( ${TARGETBIN} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS=1 )


# Special for this workflow-tester: Make the Workflow class more verbose
target_compile_definitions( ${TARGETBIN} PUBLIC VERBOSE_WORKFLOW=1 )


#
# Linking
#


# Libraries that are needed to build this executable
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( workflow-tester
  Qt5::Core
  # Qt5::Gui
  # Qt5::Widgets
  )
