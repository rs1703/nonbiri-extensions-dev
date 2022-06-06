unset(EXT_DEFINITIONS CACHE)

if(DEFINED id)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} ID="${id}")
else()
  message(FATAL_ERROR "id is not defined")
endif()

if(DEFINED name)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} NAME="${name}")
else()
  message(FATAL_ERROR "name is not defined")
endif()

if(DEFINED baseUrl)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} BASE_URL="${baseUrl}")
else()
  message(FATAL_ERROR "baseUrl is not defined")
endif()

if(DEFINED language)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} LANGUAGE="${language}")
else()
  message(FATAL_ERROR "language is not defined")
endif()

if(DEFINED version)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} VERSION="${version}")
else()
  message(FATAL_ERROR "version is not defined")
endif()

if(DEFINED isNsfw)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} IS_NSFW="${isNsfw}")
endif()

if(DEFINED useApi)
  set(EXT_DEFINITIONS ${EXT_DEFINITIONS} USE_API="${useApi}")
endif()

target_compile_definitions(${id} PRIVATE ${EXT_DEFINITIONS})
target_compile_features(${id} PRIVATE cxx_std_20)
target_link_libraries(${id} PRIVATE ${LIBRARIES})