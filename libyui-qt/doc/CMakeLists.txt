# CMakeLists.txt for libyui*/doc

include( ../../VERSION.cmake )

find_package( Doxygen REQUIRED dot )


#
# Doxygen-generated autodocs
#

if ( DOXYGEN_FOUND )

  set( DOXYGEN_GENERATE_TREEVIEW yes ) # Enable views tree HTML frame
  set( DOXYGEN_QUIET             yes ) # Less verbose output
  set( DOXYGEN_WARN_LOGFILE      doxygen-warnings.log )
  # See build/CMakeDoxyfile.in for more supported variables

  doxygen_add_docs( doc ../src )

else()
  message( WARNING "Missing doxygen package" )
endif()
