cmake_minimum_required(VERSION 3.15)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.24.0.tar.gz"
  SHA1 "a3d7f4372b1dcd52faa6ff4a3bd5358e1d0e5efd"
  LOCAL)

project(index LANGUAGES C CXX)
include(FindPkgConfig)

# Main =======================================

include_directories(${CMAKE_CURRENT_LIST_DIR})

file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/*.cpp)
add_executable(${PROJECT_NAME} ${DEPS} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Packages ===================================

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

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES} -ldl)
endif()
