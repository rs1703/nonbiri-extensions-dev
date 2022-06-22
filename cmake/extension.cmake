if(DEFINED domain)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_DOMAIN_NAME="${domain}")
else()
  message(FATAL_ERROR "domain is not defined")
endif()

if(DEFINED baseUrl)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_BASE_URL="${baseUrl}")
else()
  message(FATAL_ERROR "baseUrl is not defined")
endif()

if(DEFINED name)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_NAME="${name}")
else()
  message(FATAL_ERROR "name is not defined")
endif()

if(DEFINED description)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_DESCRIPTION="${description}")
endif()

if(DEFINED language)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_LANGUAGE="${language}")
else()
  message(FATAL_ERROR "language is not defined")
endif()

if(DEFINED version)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_VERSION="${baseVer}.${version}")
else()
  message(FATAL_ERROR "version is not defined")
endif()

if(DEFINED isNsfw)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} EXTENSION_NSFW=1)
endif()

file(GLOB EXT_SOURCES ${CURRENT_EXT_DIR}/*.cpp)
add_library(${domain} SHARED ${DEPENDENCIES} ${EXT_SOURCES} ${EXT_CUSTOM_SOURCES})
if(WIN32)
  target_link_libraries(${domain} PRIVATE ${LIBRARIES})
else()
  target_link_libraries(${domain} PRIVATE ${LIBRARIES} -ldl)
endif()

target_compile_definitions(${domain} PRIVATE ${EXT_DEFINITIONS})
target_compile_features(${domain} PRIVATE cxx_std_20)

set_target_properties(${domain} PROPERTIES PREFIX "")
set_target_properties(${domain} PROPERTIES OUTPUT_NAME ${domain}-v${baseVer}.${version})
add_custom_command(TARGET ${domain} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CURRENT_EXT_DIR}/icon.png"
    "${CMAKE_CURRENT_BINARY_DIR}/icons/${domain}-v${baseVer}.${version}.png")