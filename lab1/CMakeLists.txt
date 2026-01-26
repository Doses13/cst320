cmake_minimum_required(VERSION 3.10)

project(Lab1Symtab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)

# Generate C++ scanner into build directory
FLEX_TARGET(langscanner
    ${CMAKE_CURRENT_SOURCE_DIR}/lang.l
    ${CMAKE_CURRENT_BINARY_DIR}/langlex.cpp
)

# Apply properties to the *correct* variable
set_source_files_properties(${FLEX_langscanner_OUTPUTS}
    PROPERTIES
        LANGUAGE CXX
        COMPILE_DEFINITIONS YY_NO_UNISTD_H
)

add_executable(lab1
    main.cpp
    cSymbol.cpp
    cSymbolTable.cpp
    ${FLEX_langscanner_OUTPUTS}
)

target_include_directories(lab1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Optional: shut up the fopen/fileno warnings
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
