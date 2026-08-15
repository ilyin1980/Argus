qt_add_executable(imageworker-gui
    main.cpp
    MainWindow.h      MainWindow.cpp      MainWindow.ui
    ResultModel.h     ResultModel.cpp
    IndexController.h IndexController.cpp
    QueryImageView.h  QueryImageView.cpp
    ThumbnailDelegate.h ThumbnailDelegate.cpp
    Theme.h             Theme.cpp
    ScanProgressBar.h   ScanProgressBar.cpp
    HelpDialog.h        HelpDialog.cpp
)

# Compiled in rather than shipped beside the executable: the manual has to be
# there in a packaged folder and after someone moves the binary.
qt_add_resources(imageworker-gui "help"
    PREFIX "/help"
    BASE "help"
    FILES
        help/user-guide.md
        help/cli-reference.md
)

set_target_properties(imageworker-gui PROPERTIES
    OUTPUT_NAME "imageworker-gui"
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE)

# uic writes ui_MainWindow.h into the autogen directory, and the promoted-widget
# includes inside it are resolved relative to the include path, not to the form.
# Without this the generated header cannot find QueryImageView.h.
target_include_directories(imageworker-gui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(imageworker-gui PRIVATE imageworker_core Qt6::Widgets)
imageworker_stage_runtime(imageworker-gui)

if(MSVC)
    target_compile_options(imageworker-gui PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(imageworker-gui PRIVATE -Wall -Wextra)
endif()
