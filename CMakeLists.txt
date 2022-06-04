cmake_minimum_required(VERSION 3.15)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
  SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
  LOCAL)

project(nonbiri-dev LANGUAGES C CXX)
include(FindPkgConfig)

# Deps =======================================

file(GLOB GUMBO_QUERY_SOURCES libs/nonbiri-core-dev/libs/gumbo-query/src/*.cpp)
file(GLOB CORE_SOURCES libs/nonbiri-core-dev/core/*.cpp libs/nonbiri-core-dev/core/*/*.cpp)

set(DEPENDENCIES
  ${GUMBO_QUERY_SOURCES}
  ${CORE_SOURCES})

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/extensions
  ${CMAKE_CURRENT_LIST_DIR}
  libs/nonbiri-core-dev/libs
  libs/nonbiri-core-dev)

# Packages ===================================

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  set(LIBRARIES ${LIBRARIES} Threads::Threads -ldl)
endif()

pkg_check_modules(CURL libcurl)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
else()
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

file(GLOB extensions extensions/*)
foreach(extension ${extensions})
  unset(id CACHE)
  unset(name CACHE)
  unset(baseUrl CACHE)
  unset(language CACHE)
  unset(version CACHE)
  unset(isNsfw CACHE)
  unset(useApi CACHE)
  
  include(${extension}/CMakeLists.txt)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/extension.cmake)
endforeach()