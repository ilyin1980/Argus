qt_add_executable(argus
    main.cpp
)

set_target_properties(argus PROPERTIES
    OUTPUT_NAME "argus"
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE)

target_link_libraries(argus PRIVATE argus_core)
argus_stage_runtime(argus)

if(MSVC)
    target_compile_options(argus PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(argus PRIVATE -Wall -Wextra)
endif()
