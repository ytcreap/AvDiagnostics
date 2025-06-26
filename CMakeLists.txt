cmake_minimum_required(VERSION 3.14)

project(DiagnosticsTest2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(HEADERS
    ${HEADER_DIR}/industrialprotocolutils.h
    ${HEADER_DIR}/avlicensing.h
    ${HEADER_DIR}/opcuaclient.h
    ${HEADER_DIR}/tinyxml2.h
)

set(SOURCES
    ${SOURCE_DIR}/industrialprotocolutils.cpp
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/tinyxml2.cpp
    ${SOURCE_DIR}/opcuaclient.cpp
    ${SOURCE_DIR}/avlicensing.cpp
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS coroutine context)

include_directories(
    ${HEADER_DIR}
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

find_library(OPEN62541_LIB NAMES open62541 PATHS ${LIB_DIR}  NO_DEFAULT_PATH)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPEN62541_LIB}
    Threads::Threads
    grdlic
    Qt${QT_VERSION_MAJOR}::Core

)
