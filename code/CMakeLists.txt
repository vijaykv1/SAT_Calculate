cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(sat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sat
    CNF.cpp
    DavisPutnam.cpp
    fileParser.cpp
    main.cpp
)

target_include_directories(sat
    PUBLIC
     ${CMAKE_CURRENT_LIST_DIR}
)
