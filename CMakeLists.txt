project(KompexSqliteWrapper CXX)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (DEFINED KOMPEX_SQLITE_WRAPPER_SHARED)
    option(KOMPEX_SQLITE_WRAPPER_SHARED "Build shared lib" ${KOMPEX_SQLITE_WRAPPER_SHARED})
else()
    option(KOMPEX_SQLITE_WRAPPER_SHARED "Build shared lib" ON)
endif()

if (DEFINED KOMPEX_SQLITE_WRAPPER_STATIC)
    option(KOMPEX_SQLITE_WRAPPER_STATIC "Build static lib" ${KOMPEX_SQLITE_WRAPPER_STATIC})
else()
    option(KOMPEX_SQLITE_WRAPPER_STATIC "Build static lib" ON)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_definitions(std=c++0x)

find_package(Sqlite3 REQUIRED)
find_package(GTest REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/gtest)
include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIR})

set(LIB_TARGET kompex-sqlite-wrapper)
set(LIB_SRC
    ./src/KompexSQLiteBlob.cpp
    ./src/KompexSQLiteDatabase.cpp
    ./src/KompexSQLiteStatement.cpp)

# Shared library
if (KOMPEX_SQLITE_WRAPPER_SHARED)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
    set(CMAKE_SHARED_LINKER_FLAGS "-export-dynamic")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
    add_library(${LIB_TARGET} SHARED ${SQLITE3_LIBRARIES} ${LIB_SRC})
endif()

#Static library
if (KOMPEX_SQLITE_WRAPPER_STATIC)
    set(LIB_STATIC_TARGET "${LIB_TARGET}-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
    add_library(${LIB_STATIC_TARGET} STATIC ${SQLITE3_LIBRARIES} ${LIB_SRC})
    set_target_properties(${LIB_STATIC_TARGET} PROPERTIES OUTPUT_NAME ${LIB_TARGET})
endif()


# Build example file.

set(EXAMPLE_TARGET example_sqlite_wrapper)
add_executable(${EXAMPLE_TARGET} ./example/main.cpp)

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(${EXAMPLE_TARGET} ${LIB_TARGET} ${SQLITE3_LIBRARIES} pthread)


# Build Google unit test

set(GTEST_TARGET unitTest)
add_executable(${GTEST_TARGET} ./gtest/test_KompexSQLiteWrapper.cpp)

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(${GTEST_TARGET} ${LIB_TARGET} ${SQLITE3_LIBRARIES} ${GTEST_BOTH_LIBRARIES} pthread)

set(GTEST_ARGS "--gtest_color=yes ")
enable_testing()
add_test(KompexSQLiteWrapperTest ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} ${GTEST_ARGS})

