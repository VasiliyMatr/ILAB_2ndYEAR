
cmake_minimum_required( VERSION 3.11 )

project( Cache VERSION 0.1 LANGUAGES CXX )

set( COMMON_COMPILER_OPTIONS
    -Wall -Wpedantic -Wunused -Wunreachable-code
)
set( DEBUG_COMPILER_OPTIONS
    -g -O1
)
set( RELEASE_COMPILER_OPTIONS
    -O3
)

set( TEST_NAME "cache-test" )
set( EXEC_NAME "cache" )
set( TARGETS ${EXEC_NAME} ${TEST_NAME} )

add_executable( ${EXEC_NAME} )
add_executable( ${TEST_NAME} )
target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/cache.cc")
target_sources( ${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/cache-test.cc" )

foreach( TARGET IN LISTS TARGETS )

    target_include_directories( ${TARGET} PRIVATE "${CMAKE_SOURCE_DIR}/headers" )
    target_compile_features( ${TARGET} PRIVATE cxx_std_17 )
    target_compile_options( ${TARGET} PRIVATE ${COMMON_COMPILER_OPTIONS} )

    option( DEBUG_BUILD "Build debug project version." ON )

    if ( DEBUG_BUILD )
        target_compile_options( ${TARGET} PRIVATE ${DEBUG_COMPILER_OPTIONS} )
    else()
        target_compile_options( ${TARGET} PRIVATE ${RELEASE_COMPILER_OPTIONS} )
    endif( DEBUG_BUILD )

endforeach()