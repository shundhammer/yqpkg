# CMakeLists.txt for libyui/src

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

find_package( Boost REQUIRED )


set( TARGETLIB          libyui )
set( TARGETLIB_BASE     yui    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui )

# if DESTDIR is set, CMAKE_INSTALL_INCLUDEDIR already contains it
# during "make install" (but not for other make targets!):
#
#    sudo make install DESTDIR=/work/foo
#
# -> the include files are installed to /work/foo/usr/include/...
# We need that for RPM builds to install everything to $RPM_BUILD_ROOT.


set( SOURCES
  YUI.cc
  YApplication.cc
  YWidgetFactory.cc
  YOptionalWidgetFactory.cc
  YSettings.cc
  YPath.cc
  YExternalWidgets.cc

  YCommandLine.cc
  YDialogSpy.cc
  YEvent.cc
  YEventFilter.cc
  YEnvVar.cc
  YItem.cc
  YIconLoader.cc
  YMacro.cc
  YMenuItem.cc
  YProperty.cc
  YShortcut.cc
  YShortcutManager.cc
  YSimpleEventHandler.cc
  YSingleChildContainerWidget.cc
  YTableHeader.cc
  YTableItem.cc
  YTreeItem.cc
  YUIException.cc
  YUILoader.cc
  YUILog.cc
  YUIPlugin.cc
  YWidgetID.cc

  YSelectionWidget.cc
  YSimpleInputField.cc

  YAlignment.cc
  YBarGraph.cc
  YBusyIndicator.cc
  YButtonBox.cc
  YCheckBox.cc
  YCheckBoxFrame.cc
  YComboBox.cc
  YContextMenu.cc
  YDateField.cc
  YDialog.cc
  YDialogHelpers.cc
  YDownloadProgress.cc
  YDumbTab.cc
  YEmpty.cc
  YFrame.cc
  YImage.cc
  YInputField.cc
  YIntField.cc
  YItemSelector.cc
  YLabel.cc
  YLayoutBox.cc
  YLogView.cc
  YMenuBar.cc
  YMenuButton.cc
  YMenuWidget.cc
  YMultiLineEdit.cc
  YMultiProgressMeter.cc
  YMultiSelectionBox.cc
  YPackageSelector.cc
  YGraph.cc
  YPartitionSplitter.cc
  YProgressBar.cc
  YPushButton.cc
  YRadioButton.cc
  YRadioButtonGroup.cc
  YReplacePoint.cc
  YRichText.cc
  YSelectionBox.cc
  YSlider.cc
  YSpacing.cc
  YSquash.cc
  YTable.cc
  YTimeField.cc
  YTimezoneSelector.cc
  YTree.cc
  YWidget.cc
  YWizard.cc

  FSize.cc
  YStringTree.cc
  YRpmGroupsTree.cc

  YPopupInternal.cc
  YPropertyEditor.cc
  )


set( HEADERS
  YUI.h
  YApplication.h
  YWidgetFactory.h
  YOptionalWidgetFactory.h
  YSettings.h
  YPath.h

  YBuiltinCaller.h
  YBothDim.h
  YChildrenManager.h
  YColor.h
  YCommandLine.h
  YDescribedItem.h
  YDialogSpy.h
  YEvent.h
  YEventFilter.h
  YEnvVar.h
  YItem.h
  YItemCustomStatus.h
  YIconLoader.h
  YMacro.h
  YMacroPlayer.h
  YMacroRecorder.h
  YMenuItem.h
  YPackageSelectorPlugin.h
  YGraphPlugin.h
  YProperty.h
  YShortcut.h
  YShortcutManager.h
  YSimpleEventHandler.h
  YSingleChildContainerWidget.h
  YTableHeader.h
  YTableItem.h
  YTreeItem.h
  YTypes.h
  YUIException.h
  YUILoader.h
  YUILog.h
  YUIPlugin.h
  YUISymbols.h
  YWidgetID.h
  YWidget_OptimizeChanges.h

  YSelectionWidget.h
  YSimpleInputField.h

  YAlignment.h
  YBarGraph.h
  YBusyIndicator.h
  YButtonBox.h
  YCheckBoxFrame.h
  YCheckBox.h
  YComboBox.h
  YContextMenu.h
  YDateField.h
  YDialog.h
  YDownloadProgress.h
  YDumbTab.h
  YEmpty.h
  YFrame.h
  YImage.h
  YInputField.h
  YIntField.h
  YItemSelector.h
  YLabel.h
  YLayoutBox.h
  YLogView.h
  YMenuBar.h
  YMenuButton.h
  YMenuWidget.h
  YMultiLineEdit.h
  YMultiProgressMeter.h
  YMultiSelectionBox.h
  YPackageSelector.h
  YGraph.h
  YPartitionSplitter.h
  YProgressBar.h
  YPushButton.h
  YRadioButtonGroup.h
  YRadioButton.h
  YReplacePoint.h
  YRichText.h
  YSelectionBox.h
  YSlider.h
  YSpacing.h
  YSquash.h
  YTable.h
  YTimeField.h
  YTimezoneSelector.h
  YTree.h
  YWidget.h
  YWizard.h

  FSize.h
  ImplPtr.h
  TreeItem.h
  YRpmGroupsTree.h
  YStringTree.h
  YTransText.h
  YExternalWidgetFactory.h
  YExternalWidgets.h
  )


# Add shared lib to be built
add_library( ${TARGETLIB} SHARED ${SOURCES} ${HEADERS} )


#
# Include directories and compile options
#

# Generate Libyui_config.h where some CMake variables are expanded
# for use in the C++ code. This file is NOT installed upon "make install".
configure_file( Libyui_config.h.in Libyui_config.h )
target_include_directories( ${TARGETLIB} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )

# Add more compile options to this target in addition to those
# added in the toplevel CMakeLists.txt.
#
# Notice that CMake will automatically add -fPIC etc. where needed,
# like for this shared lib.
### target_compile_options( ${TARGETLIB} PUBLIC "-Dfoo" )

# Show the complete compiler commands with all arguments:
#   make VERBOSE=1

# Add more compile options to an individual source file:
### set_source_files_properties( YUI.cc PROPERTIES COMPILE_OPTIONS "-Dfoo" )


#
# Linking
#

# Libraries that are needed to build this shared lib
#
# We only use the parts of Boost that are completely contained in header files
# without any binary part, so we don't need to link against any Boost lib.
# Should that become necessary, add Boost::filesystem or whatever lib is
# needed. See also
# https://cliutils.gitlab.io/modern-cmake/chapters/packages/Boost.html
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( ${TARGETLIB}
  dl
  pthread
  )


# https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#target-properties
set_target_properties( ${TARGETLIB} PROPERTIES
  VERSION       ${SONAME}           # From ../../VERSION.cmake
  SOVERSION     ${SONAME_MAJOR}     # From ../../VERSION.cmake
  OUTPUT_NAME   ${TARGETLIB_BASE}
  )


#
# Install
#

# Install the headers first so the message about the lib does not scroll away
install( FILES   ${HEADERS}   DESTINATION ${HEADERS_INSTALL_DIR} )
install( TARGETS ${TARGETLIB} LIBRARY )
