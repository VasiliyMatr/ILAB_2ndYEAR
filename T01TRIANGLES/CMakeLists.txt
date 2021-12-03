
cmake_minimum_required( VERSION 3.11 )

project( Triangles VERSION 0.1 LANGUAGES CXX )
find_package( GTest REQUIRED )

set( GEOM3D_UTEST_NAME geom3D-utests )
add_executable( ${GEOM3D_UTEST_NAME} )

set( TRIANGLES_NAME triangles )
add_executable( ${TRIANGLES_NAME} )

set( DEBUG_TRIANGLES_NAME debug-triangles )
add_executable( ${DEBUG_TRIANGLES_NAME} )

target_link_libraries( ${GEOM3D_UTEST_NAME} PRIVATE
    "${GTEST_LIBRARIES}"
    "pthread"
)

target_include_directories( ${GEOM3D_UTEST_NAME} PRIVATE
    "${GTEST_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/headers"
)

target_include_directories( ${TRIANGLES_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/headers"
)
target_include_directories( ${DEBUG_TRIANGLES_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/headers"
)

set( GEOM3D_SRC_DIR "${CMAKE_SOURCE_DIR}/source/geom3D-impl" )
set( GEOM3D_TESTS_DIR "${CMAKE_SOURCE_DIR}/source/geom3D-utests" )

set( GEOM3D_SRC_FILES
    "line-impl.cc"
    "triangle-impl.cc"
    "split-impl.cc"
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

    "domain-tests.cc"
    "split-tests.cc"
)

target_sources( ${TRIANGLES_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/source/triangles.cc"
)
target_sources( ${DEBUG_TRIANGLES_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/source/triangles.cc"
)

foreach( FILE IN LISTS GEOM3D_SRC_FILES )
    target_sources( ${GEOM3D_UTEST_NAME} PRIVATE "${GEOM3D_SRC_DIR}/${FILE}" )
    target_sources( ${TRIANGLES_NAME} PRIVATE "${GEOM3D_SRC_DIR}/${FILE}" )
    target_sources( ${DEBUG_TRIANGLES_NAME} PRIVATE "${GEOM3D_SRC_DIR}/${FILE}" )
endforeach()

foreach( FILE IN LISTS GEOM3D_TESTS_FILES )
    target_sources( ${GEOM3D_UTEST_NAME} PRIVATE "${GEOM3D_TESTS_DIR}/${FILE}" )
endforeach()

set( COMMON_FLAGS -Wall -Wextra -Wpedantic -Werror )
set( DEBUG_FLAGS -O0 -g ${COMMON_FLAGS} )
# Release build works much faster with COMMON_FLAGS. WHUT?
set( RELEASE_FLAGS -DNDEBUG -O3 ${COMMON_FLAGS} )

target_compile_features( ${GEOM3D_UTEST_NAME} PRIVATE cxx_std_20 )
target_compile_features( ${TRIANGLES_NAME} PRIVATE cxx_std_20 )
target_compile_features( ${DEBUG_TRIANGLES_NAME} PRIVATE cxx_std_20 )

target_compile_options( ${GEOM3D_UTEST_NAME} PRIVATE ${DEBUG_FLAGS} )
target_compile_options( ${DEBUG_TRIANGLES_NAME} PRIVATE ${DEBUG_FLAGS} )
target_compile_options( ${TRIANGLES_NAME} PRIVATE ${COMMON_FLAGS} )

# Testing stuff
set( GEN_OUTPUT_NAME out.txt )
set( GEN_SCRIPT_NAME ${CMAKE_HOME_DIRECTORY}/genTr.py )
execute_process( COMMAND python3 ${GEN_SCRIPT_NAME} OUTPUT_FILE ${GEN_OUTPUT_NAME} )

enable_testing()

add_test( NAME UnitTests COMMAND
    ${GEOM3D_UTEST_NAME}
)

add_test( NAME ValgrindTest COMMAND
    sh -c "valgrind --leak-check=full --error-exitcode=1 ${VALGRIND_FLAGS} ./${DEBUG_TRIANGLES_NAME} < ${GEN_OUTPUT_NAME}"
)
add_test( NAME SpeedTest COMMAND
    sh -c "./${TRIANGLES_NAME} < ${GEN_OUTPUT_NAME}"
)
set_tests_properties( SpeedTest PROPERTIES TIMEOUT 2 )
