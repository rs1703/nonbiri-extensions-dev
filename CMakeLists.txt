cmake_minimum_required(VERSION 3.5)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.297.tar.gz"
  SHA1 "3319fe6a3b08090df7df98dee75134d68e2ef5a3"
)

project(nonbiri-extensions-dev)
enable_language(C CXX)

hunter_add_package(CURL)
hunter_add_package(gumbo)

find_package(CURL CONFIG REQUIRED)
find_package(gumbo CONFIG REQUIRED)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev
  ${CMAKE_CURRENT_LIST_DIR}/extensions
  ${CURL_ROOT}/include
  ${GUMBO_ROOT}/include
)

link_libraries(CURL::libcurl gumbo::gumbo)

set(deps
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Document.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Node.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Object.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Parser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/QueryUtil.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Selection.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/Selector.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/core/extension/extension.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/core/parser/parser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/core/utils/utils.cpp
)

file(GLOB extensions extensions/*)
foreach(extension ${extensions})
  set(id "")
  set(name "")
  set(language en)
  set(version 0.1.0)
  set(useApi 0)
  
  include(${extension}/CMakeLists.txt)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/extension.cmake)
endforeach()
