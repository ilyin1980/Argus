qt_add_executable(imageworker
    main.cpp
)

set_target_properties(imageworker PROPERTIES
    OUTPUT_NAME "imageworker"
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE)

target_link_libraries(imageworker PRIVATE imageworker_core)
imageworker_stage_runtime(imageworker)

if(MSVC)
    target_compile_options(imageworker PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(imageworker PRIVATE -Wall -Wextra)
endif()
