cmake_minimum_required(VERSION 3.2)

set(extName Koushoku)
set(libName koushoku)
set(version 0.1.0)
set(language en)

set(baseUrl https://koushoku.org)

add_library(${libName} SHARED ${deps}
    ${CMAKE_CURRENT_LIST_DIR}/koushoku.cpp 
)