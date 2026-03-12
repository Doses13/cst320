cmake_minimum_required(VERSION 3.10)

project(Lab4AST LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BISON_EXECUTABLE "C:/Users/shygu/source/repos/win_flex_bison-2.5.25/win_bison.exe")
#set(FLEX_EXECUTABLE "C:/Users/shygu/source/repos/win_flex_bison-2.5.25/win_flex.exe")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# -----------------------------
# Generate parser with Bison
# -----------------------------
BISON_TARGET(langparser
    ${CMAKE_CURRENT_SOURCE_DIR}/lang.y
    ${CMAKE_CURRENT_BINARY_DIR}/langparse.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/langparse.h
)

# -----------------------------
# Generate scanner with Flex
# -----------------------------
FLEX_TARGET(langscanner
    ${CMAKE_CURRENT_SOURCE_DIR}/lang.l
    ${CMAKE_CURRENT_BINARY_DIR}/langlex.cpp
)

# Make sure scanner is rebuilt after parser if needed
ADD_FLEX_BISON_DEPENDENCY(langscanner langparser)

# Treat generated scanner as C++ and suppress Windows unistd issue
set_source_files_properties(
    ${FLEX_langscanner_OUTPUTS}
    PROPERTIES
        LANGUAGE CXX
        COMPILE_DEFINITIONS YY_NO_UNISTD_H
)

# -----------------------------
# Build executable
# -----------------------------
add_executable(lab4
    main.cpp
    cVisitor.cpp
    cSymbolTable.cpp
    ${BISON_langparser_OUTPUTS}
    ${FLEX_langscanner_OUTPUTS}
)

# Source dir for headers like astnodes.h / cSymbolTable.h
# Binary dir for generated langparse.h
target_include_directories(lab4 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Optional Windows warning cleanup
target_compile_definitions(lab4 PRIVATE
    _CRT_SECURE_NO_WARNINGS
)