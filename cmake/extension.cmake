target_compile_definitions(
    ${id} PRIVATE
    ID="${id}"
    NAME="${name}"
    BASE_URL="${baseUrl}"
    LANGUAGE="${language}"
    VERSION="${version}"
    IS_NSFW=${isNsfw}
    USE_API=${useApi}
)
target_compile_features(${id} PRIVATE cxx_std_17)