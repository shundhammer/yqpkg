# CMakeLists.txt for libyui/legacy-buildtools
#
# Support for the legacy libyui cmake environment:
# Install the content of the legacy-buildtools/ subdir to the system.
#
# We will need this until all the community-supported libyui plugins and
# extensions (libyui-gtk, libyui-mga, libyui-mga-qt, ...) are ported to this
# new cmake environment.
#
# See
#
#   https://gist.github.com/shundhammer/d96a63a1cab8245d20c2e3556238a114
#
# When this is removed, remember to also remove it from the files list in the
# .spec file!

set( BUILDTOOLS_INSTALL_DIR ${DESTDIR}${CMAKE_INSTALL_PREFIX}/share/libyui/buildtools )

set( BUILDTOOLS_FILES
  BuildTreeSettings.cmake.in
  CMakeLists.common
  Config.cmake.in
  config.h.in
  ConfigVersion.cmake.in
  Doxyfile.in
  FindGTK3.cmake
  LibyuiCommon.cmake
  template-doc.spec.in
  template.pc.in )

install( FILES ${BUILDTOOLS_FILES} DESTINATION ${BUILDTOOLS_INSTALL_DIR} )
