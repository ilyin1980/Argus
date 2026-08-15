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
    Localization.h      Localization.cpp
    BranchDialog.h      BranchDialog.cpp
)

# Translations. Compiled here and embedded, not shipped as loose files: a
# catalogue that can go missing produces a half-translated window, which is
# worse than an untranslated one.
#
# LinguistTools is optional on purpose. Several Linux distributions split
# lrelease into a package that a plain "qt6-base-dev" install does not pull in,
# and refusing to build a working tool over a missing translator is the wrong
# trade. Without it the binary is English-only and says so at configure time.
set(IMAGEWORKER_LANGUAGES ar de es fr it ja ko nl pl pt_BR tr uk zh_CN)
list(TRANSFORM IMAGEWORKER_LANGUAGES
     PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../translations/imageworker_"
     OUTPUT_VARIABLE IMAGEWORKER_TS_FILES)
list(TRANSFORM IMAGEWORKER_TS_FILES APPEND ".ts")

find_package(Qt6 QUIET COMPONENTS LinguistTools)
if(Qt6LinguistTools_FOUND)
    qt_add_lrelease(imageworker-gui
        TS_FILES ${IMAGEWORKER_TS_FILES}
        QM_FILES_OUTPUT_VARIABLE IMAGEWORKER_QM_FILES)
    qt_add_resources(imageworker-gui "i18n"
        PREFIX "/i18n"
        BASE "${CMAKE_CURRENT_BINARY_DIR}"
        FILES ${IMAGEWORKER_QM_FILES})

    # lupdate is a developer action, never part of a build: it rewrites the .ts
    # sources, and a build that edits its own inputs is a build that cannot be
    # trusted twice.
    qt_add_lupdate(imageworker-gui
        TS_FILES ${IMAGEWORKER_TS_FILES}
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}
        NO_GLOBAL_TARGET)
else()
    message(STATUS
        "Qt LinguistTools not found - imageworker-gui will be English only. "
        "Install the Qt Linguist tools (Debian/Ubuntu: qt6-tools-dev-tools) to "
        "build the translations.")
endif()

# Compiled in rather than shipped beside the executable: the manual has to be
# there in a packaged folder and after someone moves the binary.
#
# Globbed because the manual comes in one file per language, and a list that
# has to be edited by hand is a list that will one day be missing a language
# nobody notices until a reader gets an English page.
file(GLOB IMAGEWORKER_HELP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
     CONFIGURE_DEPENDS "help/*.md")
qt_add_resources(imageworker-gui "help"
    PREFIX "/help"
    BASE "help"
    FILES ${IMAGEWORKER_HELP_FILES}
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
