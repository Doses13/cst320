cmake_minimum_required(VERSION 3.10)

project(Lab7CodeGen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BISON_EXECUTABLE "C:/Users/shygu/source/repos/win_flex_bison-2.5.25/win_bison.exe")
#set(FLEX_EXECUTABLE "C:/Users/shygu/source/repos/win_flex_bison-2.5.25/win_flex.exe")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(langparser
    ${CMAKE_CURRENT_SOURCE_DIR}/lang.y
    ${CMAKE_CURRENT_BINARY_DIR}/langparse.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/langparse.h
)

FLEX_TARGET(langscanner
    ${CMAKE_CURRENT_SOURCE_DIR}/lang.l
    ${CMAKE_CURRENT_BINARY_DIR}/langlex.cpp
)

ADD_FLEX_BISON_DEPENDENCY(langscanner langparser)

set_source_files_properties(
    ${FLEX_langscanner_OUTPUTS}
    PROPERTIES
        LANGUAGE CXX
        COMPILE_DEFINITIONS YY_NO_UNISTD_H
)

add_executable(lab7
    main.cpp
    cSemantics.cpp
    cSymbol.cpp
    cSymbolTable.cpp
    cVisitor.cpp
    cSizeOffset.cpp
    emit.cpp
    cCodeGen.cpp
    ${BISON_langparser_OUTPUTS}
    ${FLEX_langscanner_OUTPUTS}
)

target_include_directories(lab7 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(lab7 PRIVATE
    _CRT_SECURE_NO_WARNINGS
)