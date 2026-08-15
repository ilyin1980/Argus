# The Windows icon comes in through a resource script, which is the only way an
# executable carries one; the macOS bundle takes a file, and Linux takes a
# desktop entry installed beside the binary. Three platforms, three mechanisms,
# one picture.
set(ARGUS_WINDOWS_RESOURCES)
if(WIN32)
    set(ARGUS_WINDOWS_RESOURCES branding/argus.rc)
endif()

qt_add_executable(argus-gui
    main.cpp
    ${ARGUS_WINDOWS_RESOURCES}
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
set(ARGUS_LANGUAGES ar de es fr it ja ko nl pl pt_BR tr uk zh_CN)
list(TRANSFORM ARGUS_LANGUAGES
     PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../translations/argus_"
     OUTPUT_VARIABLE ARGUS_TS_FILES)
list(TRANSFORM ARGUS_TS_FILES APPEND ".ts")

find_package(Qt6 QUIET COMPONENTS LinguistTools)
if(Qt6LinguistTools_FOUND)
    qt_add_lrelease(argus-gui
        TS_FILES ${ARGUS_TS_FILES}
        QM_FILES_OUTPUT_VARIABLE ARGUS_QM_FILES)
    qt_add_resources(argus-gui "i18n"
        PREFIX "/i18n"
        BASE "${CMAKE_CURRENT_BINARY_DIR}"
        FILES ${ARGUS_QM_FILES})

    # lupdate is a developer action, never part of a build: it rewrites the .ts
    # sources, and a build that edits its own inputs is a build that cannot be
    # trusted twice.
    qt_add_lupdate(argus-gui
        TS_FILES ${ARGUS_TS_FILES}
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}
        NO_GLOBAL_TARGET)
else()
    message(STATUS
        "Qt LinguistTools not found - argus-gui will be English only. "
        "Install the Qt Linguist tools (Debian/Ubuntu: qt6-tools-dev-tools) to "
        "build the translations.")
endif()

# Compiled in rather than shipped beside the executable: the manual has to be
# there in a packaged folder and after someone moves the binary.
#
# Globbed because the manual comes in one file per language, and a list that
# has to be edited by hand is a list that will one day be missing a language
# nobody notices until a reader gets an English page.
file(GLOB ARGUS_HELP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
     CONFIGURE_DEPENDS "help/*.md")
qt_add_resources(argus-gui "branding"
    PREFIX "/branding"
    BASE "branding"
    FILES
        branding/argus-logo.png
        branding/argus-icon-256.png
)

qt_add_resources(argus-gui "help"
    PREFIX "/help"
    BASE "help"
    FILES ${ARGUS_HELP_FILES}
)

set_target_properties(argus-gui PROPERTIES
    OUTPUT_NAME "argus-gui"
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Argus"
    MACOSX_BUNDLE_GUI_IDENTIFIER "games.ab.argus"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_ICON_FILE "argus.icns")

if(APPLE)
    # The icns has to be inside the bundle's Resources, and marked as such, or
    # Info.plist names a file that is not there and Finder shows a blank page.
    target_sources(argus-gui PRIVATE branding/argus.icns)
    set_source_files_properties(branding/argus.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(UNIX AND NOT APPLE)
    install(FILES branding/argus.desktop
            DESTINATION share/applications OPTIONAL)
    install(FILES branding/argus-icon-256.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME argus.png OPTIONAL)
endif()

# uic writes ui_MainWindow.h into the autogen directory, and the promoted-widget
# includes inside it are resolved relative to the include path, not to the form.
# Without this the generated header cannot find QueryImageView.h.
target_include_directories(argus-gui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(argus-gui PRIVATE argus_core Qt6::Widgets)
argus_stage_runtime(argus-gui)

if(MSVC)
    target_compile_options(argus-gui PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(argus-gui PRIVATE -Wall -Wextra)
endif()
