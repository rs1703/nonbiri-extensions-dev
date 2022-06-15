cmake_minimum_required(VERSION 3.15)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
  SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
  LOCAL)

project(nonbiri-dev LANGUAGES C CXX)
include(FindPkgConfig)
set(baseVer 1.0)

# Deps =======================================

file(GLOB CORE_SOURCES 
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/core/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/core/*/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs/gumbo-query/src/*.cpp)
set(DEPENDENCIES ${CORE_SOURCES})

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/extensions
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev
  ${CMAKE_CURRENT_LIST_DIR}/libs/nonbiri-core-dev/libs)

# Packages ===================================

pkg_check_modules(CURL libcurl)
if(NOT CURL_FOUND)
  unset(CURL_FOUND CACHE)
  hunter_add_package(CURL)
  find_package(CURL CONFIG REQUIRED)
endif()

pkg_check_modules(JSONCPP jsoncpp)
if(JSONCPP_FOUND)
  include_directories(${JSONCPP_INCLUDE_DIR})

  set(LIBRARIES ${LIBRARIES} ${JSONCPP_LIBRARIES})
else()
  unset(JSONCPP_FOUND CACHE)
  hunter_add_package(jsoncpp)
  find_package(jsoncpp CONFIG REQUIRED)

  set(LIBRARIES ${LIBRARIES} jsoncpp_lib_static)
endif()

pkg_check_modules(GUMBO gumbo)
if(GUMBO_FOUND)
  include_directories(${GUMBO_INCLUDE_DIR})

  set(LIBRARIES ${LIBRARIES} ${GUMBO_LIBRARIES})
else()
  unset(GUMBO_FOUND CACHE)
  hunter_add_package(gumbo)
  find_package(gumbo CONFIG REQUIRED)

  set(LIBRARIES ${LIBRARIES} gumbo::gumbo)
endif()

# Extensions =======================================

file(GLOB EXT_DIRS ${CMAKE_CURRENT_LIST_DIR}/extensions/*)
foreach(CURRENT_EXT_DIR ${EXT_DIRS})
  unset(EXT_SOURCES)
  unset(EXT_DEFINITIONS)

  unset(domain)
  unset(baseUrl)
  unset(name)
  unset(language)
  unset(version)
  unset(isNsfw)
  
  include(${CURRENT_EXT_DIR}/CMakeLists.txt)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/extension.cmake)
endforeach()