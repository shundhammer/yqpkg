# To run the tests, use
#   make test
# or
#   make test ARGS=-VV
# see also
#   man ctest

set(BASELIB "yui")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ../src ${CMAKE_INCLUDE_PATH} )
add_definitions(-DBOOST_TEST_DYN_LINK -DTESTS_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

link_libraries(
  ${BASELIB}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
link_directories(BEFORE PUBLIC ../build/src)

file(GLOB unit_tests "*_test.cc")
foreach(unit_test ${unit_tests})
  get_filename_component(unit_test_bin ${unit_test} NAME_WE)

  add_executable(${unit_test_bin} ${unit_test})
  add_dependencies(${unit_test_bin} "lib${BASELIB}")

  add_test(NAME ${unit_test_bin} COMMAND ${unit_test_bin})
endforeach(unit_test)
