cmake_minimum_required(VERSION 3.15)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
  SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
  LOCAL)

project(nonbiri-dev LANGUAGES C CXX)
include(FindPkgConfig)

# Packages ===================================

add_definitions(-DCURL_STATICLIB)
pkg_check_modules(CURL libcurl)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})

  link_libraries(${CURL_LIBRARIES})
else()
  unset(CURL_FOUND CACHE)
  hunter_add_package(CURL)
  find_package(CURL CONFIG REQUIRED)

  link_libraries(CURL::libcurl)
endif()

pkg_check_modules(JSONCPP jsoncpp)
if(JSONCPP_FOUND)
  include_directories(${JSONCPP_INCLUDE_DIR})

  link_libraries(${JSONCPP_LIBRARIES})
else()
  unset(JSONCPP_FOUND CACHE)
  hunter_add_package(jsoncpp)
  find_package(jsoncpp CONFIG REQUIRED)

  link_libraries(jsoncpp_lib_static)
endif()

pkg_check_modules(GUMBO gumbo)
if(GUMBO_FOUND)
  include_directories(${GUMBO_INCLUDE_DIR})

  link_libraries(${GUMBO_LIBRARIES})
else()
  unset(GUMBO_FOUND CACHE)
  hunter_add_package(gumbo)
  find_package(gumbo CONFIG REQUIRED)

  link_libraries(gumbo::gumbo)
endif()

# Deps =======================================

file(GLOB GUMBO_QUERY_SOURCES libs/nonbiri-core-dev/libs/gumbo-query/src/*.cpp)
set(DEPS
  ${GUMBO_QUERY_SOURCES}
  libs/nonbiri-core-dev/core/extension/extension.cpp
  libs/nonbiri-core-dev/core/parser/parser.cpp
  libs/nonbiri-core-dev/core/utils/utils.cpp)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/extensions
  ${CMAKE_CURRENT_LIST_DIR}
  libs/nonbiri-core-dev/libs
  libs/nonbiri-core-dev)

# Extensions =======================================

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