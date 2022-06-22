cmake_minimum_required(VERSION 3.15)

include(${CURRENT_EXT_DIR}/../ehentai/CMakeLists.txt)

set(domain exhentai.org)
set(baseUrl https://exhentai.org)
set(name ExHentai)

file(GLOB EXT_CUSTOM_SOURCES ${CURRENT_EXT_DIR}/../ehentai/*.cpp)
