cmake_minimum_required(VERSION 3.15)

set(id mangadex)
set(name MangaDex)
set(language multilingual)
set(version 0.1.0)
set(baseUrl https://mangadex.org)
set(isNsfw 1)
set(useApi 1)

add_library(${id} SHARED ${DEPENDENCIES}
  ${CMAKE_CURRENT_LIST_DIR}/mangadex.cpp
  ${CMAKE_CURRENT_LIST_DIR}/models.cpp
  ${CMAKE_CURRENT_LIST_DIR}/utility.cpp
)