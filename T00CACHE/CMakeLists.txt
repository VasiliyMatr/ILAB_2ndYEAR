
cmake_minimum_required( VERSION 3.11 )

project( Cache VERSION 0.1 LANGUAGES C )

set( EXEC_NAME "cache-test" )

add_executable( ${EXEC_NAME} )
target_include_directories( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/headers" )
target_sources( ${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/cache-main.cc" )

set( COMMON_COMPILER_OPTIONS
    -Wall -Wpedantic -Wunused -Wunreachable-code
)
set( DEBUG_COMPILER_OPTIONS
    -g -O1
)
set( RELEASE_COMPILER_OPTIONS
    -O3
)

target_compile_features( ${EXEC_NAME} PRIVATE cxx_std_17 )
target_compile_options( ${EXEC_NAME} PRIVATE ${COMMON_COMPILER_OPTIONS} )

option( DEBUG_BUILD "Build debug project version." ON )

if ( DEBUG_BUILD )
    target_compile_options( ${EXEC_NAME} PRIVATE ${DEBUG_COMPILER_OPTIONS} )
else()
    target_compile_options( ${EXEC_NAME} PRIVATE ${RELEASE_COMPILER_OPTIONS} )
endif( DEBUG_BUILD )
