cmake_minimum_required(VERSION 3.15)

set(id koushoku)
set(name Koushoku)
set(language en)
set(version 0.1.0)
set(baseUrl https://koushoku.org)

add_library(${id} SHARED ${DEPS}
    ${CMAKE_CURRENT_LIST_DIR}/koushoku.cpp)