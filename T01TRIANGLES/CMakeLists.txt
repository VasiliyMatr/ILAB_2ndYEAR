
cmake_minimum_required( VERSION 3.11 )

project( Triangles VERSION 0.1 LANGUAGES CXX )

find_package( GTest REQUIRED )

set( GEOM3D_TEST_NAME "geom3D-tests" )
add_executable( ${GEOM3D_TEST_NAME} )

target_include_directories( ${GEOM3D_TEST_NAME} PRIVATE
    "${GTEST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/headers"
)

target_sources( ${GEOM3D_TEST_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/source/geom3D.cc"
    "${CMAKE_SOURCE_DIR}/source/${GEOM3D_TEST_NAME}.cc"
)

target_link_libraries( ${GEOM3D_TEST_NAME} PRIVATE
    "${GTEST_LIBRARIES}"
    "pthread"
)

target_compile_options( ${GEOM3D_TEST_NAME} PRIVATE
    -O1 -g -Wall
)