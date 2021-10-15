
cmake_minimum_required( VERSION 3.11 )

project( Triangles VERSION 0.1 LANGUAGES CXX )

find_package( GTest REQUIRED )

set( DEBUG_DEFINE_NAME "DEBUG" )
set( CXX_STD_OPTION "--std=c++17" )
set( DEBUG_FLAGS ${CXX_STD_OPTION} -D${DEBUG_DEFINE_NAME}=1 -O0 -g -Wall -Wextra -Wpedantic  )
set( RELEASE_FLAGS ${CXX_STD_OPTION} -D${DEBUG_DEFINE_NAME}=0 -O2 )
option( DEBUG_BUILD "Should build debug exec version?" ON )

set( GEOM3D_TEST_NAME "geom3D-tests" )
add_executable( ${GEOM3D_TEST_NAME} )

set( TRIANGLES_NAME "triangles" )
add_executable( ${TRIANGLES_NAME} )

set( GEOM3D_SRC_DIR "${CMAKE_SOURCE_DIR}/source/geom3D-impl" )
set( GEOM3D_TESTS_DIR "${CMAKE_SOURCE_DIR}/source/geom3D-tests" )

target_include_directories( ${GEOM3D_TEST_NAME} PRIVATE
    "${GTEST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/headers"
)

target_include_directories( ${TRIANGLES_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/headers"
)

set( GEOM3D_SRC_FILES
    "line-impl.cc"
    "plane-impl.cc"
    "point-impl.cc"
    "seg-impl.cc"
    "utils-impl.cc"
    "vec-impl.cc"
    "triangle-impl.cc"
)

set( GEOM3D_TESTS_FILES
    "line-tests.cc"
    "plane-tests.cc"
    "point-tests.cc"
    "seg-tests.cc"
    "tests-main.cc"
    "utils-tests.cc"
    "vec-tests.cc"
    "triangle-tests.cc"
)

target_sources( ${TRIANGLES_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/triangles.cc" )

foreach( FILE IN LISTS GEOM3D_SRC_FILES )
    target_sources( ${GEOM3D_TEST_NAME} PRIVATE "${GEOM3D_SRC_DIR}/${FILE}" )
    target_sources( ${TRIANGLES_NAME} PRIVATE "${GEOM3D_SRC_DIR}/${FILE}" )
endforeach()

foreach( FILE IN LISTS GEOM3D_TESTS_FILES )
    target_sources( ${GEOM3D_TEST_NAME} PRIVATE "${GEOM3D_TESTS_DIR}/${FILE}" )
endforeach()

target_link_libraries( ${GEOM3D_TEST_NAME} PRIVATE
    "${GTEST_LIBRARIES}"
    "pthread"
)

target_compile_options( ${GEOM3D_TEST_NAME} PRIVATE
    ${DEBUG_FLAGS}
)

target_compile_options( ${TRIANGLES_NAME} PRIVATE
    ${RELEASE_FALGS}
)