target_compile_definitions(
    ${libName} PRIVATE
    BASE_URL="${baseUrl}"
    NAME="${extName}"
    LANGUAGE="${language}"
    VERSION="${version}"
    USE_API="${useApi}"
)
target_compile_features(${libName} PRIVATE cxx_std_17)