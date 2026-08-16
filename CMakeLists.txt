cmake_minimum_required(VERSION 3.21)

project(Argus
    VERSION 0.1.0
    DESCRIPTION "Duplicate / similar image finder — worker prototype"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ARGUS_BUILD_CLI       "Build the headless CLI"                      ON)
option(ARGUS_BUILD_GUI       "Build the Qt Widgets GUI"                    ON)
option(ARGUS_WITH_INFERENCE  "Enable ONNX Runtime embedding backend (P2)"  OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
    # DirectML and ONNX Runtime pull in windows.h, whose min/max macros would
    # otherwise shadow std::min and std::max at every call site.
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Sql)
if(ARGUS_BUILD_GUI)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
endif()

qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Optional neural backend. Everything lives under third_party/ so that no
# system-wide installation is required and nothing lands on the C: drive.
# ---------------------------------------------------------------------------
if(ARGUS_WITH_INFERENCE)
    set(ARGUS_ORT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/onnxruntime"
        CACHE PATH "ONNX Runtime root")
    set(ARGUS_OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv/build"
        CACHE PATH "Prebuilt OpenCV root; ignored when OpenCV comes from the system")

    if(NOT EXISTS "${ARGUS_ORT_ROOT}/include/onnxruntime_cxx_api.h")
        message(FATAL_ERROR "ONNX Runtime not found at ${ARGUS_ORT_ROOT}")
    endif()

    add_library(onnxruntime SHARED IMPORTED GLOBAL)
    if(WIN32)
        set_target_properties(onnxruntime PROPERTIES
            IMPORTED_LOCATION "${ARGUS_ORT_ROOT}/bin/onnxruntime.dll"
            IMPORTED_IMPLIB   "${ARGUS_ORT_ROOT}/lib/onnxruntime.lib")
    else()
        # The Linux and macOS tarballs ship the shared library directly, with no
        # separate import stub.
        file(GLOB _ort_lib
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime.so"
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime.dylib")
        if(NOT _ort_lib)
            message(FATAL_ERROR "No ONNX Runtime shared library in ${ARGUS_ORT_ROOT}/lib")
        endif()
        list(GET _ort_lib 0 _ort_lib)
        set_target_properties(onnxruntime PROPERTIES IMPORTED_LOCATION "${_ort_lib}")
    endif()
    set_target_properties(onnxruntime PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ARGUS_ORT_ROOT}/include")

    # Prefer a prebuilt OpenCV when one was vendored; fall back to the system
    # package, which is how it arrives on Linux.
    if(EXISTS "${ARGUS_OPENCV_DIR}")
        find_package(OpenCV REQUIRED PATHS "${ARGUS_OPENCV_DIR}" NO_DEFAULT_PATH)
    else()
        find_package(OpenCV REQUIRED)
    endif()

    # Runtime dependencies that must sit next to the executables.
    set(ARGUS_RUNTIME_DLLS
        "${ARGUS_ORT_ROOT}/bin/onnxruntime.dll"
        "${ARGUS_ORT_ROOT}/bin/onnxruntime_providers_shared.dll"
        "${ARGUS_ORT_ROOT}/bin/DirectML.dll")
    foreach(_cvdll IN LISTS OpenCV_LIBS)
        # opencv_world ships one DLL; pick it up from the known bin directory.
    endforeach()
    file(GLOB _opencv_dlls "${ARGUS_OPENCV_DIR}/x64/vc16/bin/opencv_world*[0-9].dll")
    list(APPEND ARGUS_RUNTIME_DLLS ${_opencv_dlls})

    message(STATUS "  OpenCV          : ${OpenCV_VERSION} (${OpenCV_DIR})")
endif()

##
# @brief Copy the shared libraries a target needs into its output directory.
#
function(argus_stage_runtime target)
    if(NOT ARGUS_WITH_INFERENCE)
        return()
    endif()
    foreach(_dll IN LISTS ARGUS_RUNTIME_DLLS)
        if(EXISTS "${_dll}")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_dll}" "$<TARGET_FILE_DIR:${target}>"
                VERBATIM)
        endif()
    endforeach()
endfunction()

add_subdirectory(src/core)

if(ARGUS_BUILD_CLI)
    add_subdirectory(src/cli)
endif()

if(ARGUS_BUILD_GUI)
    add_subdirectory(src/gui)
endif()

# ---------------------------------------------------------------------------
# Packaging: a folder that runs anywhere, with every dependency beside the
# executables. Fully static linking is not on the table — ONNX Runtime with
# DirectML ships as a DLL only, and DirectML.dll is a Microsoft redistributable
# that cannot be linked at all — so "self-contained" means "nothing to install",
# not "one file".
# ---------------------------------------------------------------------------
option(ARGUS_PACKAGE_MODELS "Copy the ONNX models into the package" ON)

# The stock default is a system directory the build has no right to write to,
# which turns `--target install` into a permissions error rather than a package.
# The presets set installDir, but that only helps a preset build; a bare
# `cmake -S . -B build` still lands on the default, so catch it here too.
#
# CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT is only true on the *first*
# configure of a build directory, so testing it alone silently stops working the
# moment a stale cache exists — which is exactly how an install kept failing
# while an old package sat in dist/ looking current.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT
   OR CMAKE_INSTALL_PREFIX MATCHES "^C:/Program Files"
   OR CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dist"
        CACHE PATH "Where the self-contained package is assembled" FORCE)
endif()

install(TARGETS argus RUNTIME DESTINATION .)
if(ARGUS_BUILD_GUI)
    # On macOS the GUI is a bundle, and CMake refuses to install one without
    # being told where bundles go — RUNTIME DESTINATION alone is a hard error
    # there while being perfectly sufficient everywhere else.
    install(TARGETS argus-gui
            RUNTIME DESTINATION .
            BUNDLE  DESTINATION .)
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION .)

# The C++ runtime, as libraries rather than as an installer to run.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
endif()

if(ARGUS_WITH_INFERENCE)
    foreach(_dll IN LISTS ARGUS_RUNTIME_DLLS)
        if(EXISTS "${_dll}")
            install(FILES "${_dll}" DESTINATION .)
        endif()
    endforeach()

    # ARGUS_RUNTIME_DLLS is a Windows list, so on Linux and macOS nothing above
    # ships the ONNX Runtime library at all. Both packages therefore built,
    # weighed the right amount, and died on the first line:
    #
    #   ./argus: error while loading shared libraries: libonnxruntime.so.1
    #   dyld: Library not loaded: @rpath/libonnxruntime.1.24.1.dylib
    #
    # A package that cannot start is worse than no package, because it looks
    # like one. The library goes in beside the executables, and the rpath below
    # is what lets them find it there.
    if(NOT WIN32)
        file(GLOB _ort_runtime
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime.so*"
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime.*dylib"
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime_providers_*.so"
             "${ARGUS_ORT_ROOT}/lib/libonnxruntime_providers_*.dylib")
        if(_ort_runtime)
            # PROGRAMS, not FILES: these are executable shared objects, and the
            # symlink members of the set have to keep their permissions.
            install(PROGRAMS ${_ort_runtime} DESTINATION .)
            if(APPLE AND ARGUS_BUILD_GUI)
                # The bundle looks beside itself, not beside the CLI.
                install(PROGRAMS ${_ort_runtime}
                        DESTINATION "argus-gui.app/Contents/Frameworks")
            endif()
        else()
            message(WARNING "No ONNX Runtime shared library found under"
                            " ${ARGUS_ORT_ROOT}/lib — the package will not start.")
        endif()
    endif()
endif()

# Where a packaged executable looks for the shared libraries shipped with it.
# Without this the loader searches only the system paths, finds nothing, and
# the binary never reaches main().
if(APPLE)
    set_target_properties(argus PROPERTIES INSTALL_RPATH "@executable_path")
    if(ARGUS_BUILD_GUI)
        set_target_properties(argus-gui PROPERTIES
                              INSTALL_RPATH "@executable_path/../Frameworks")
    endif()
elseif(UNIX)
    set_target_properties(argus PROPERTIES INSTALL_RPATH "$ORIGIN")
    if(ARGUS_BUILD_GUI)
        set_target_properties(argus-gui PROPERTIES INSTALL_RPATH "$ORIGIN")
    endif()
endif()

if(ARGUS_PACKAGE_MODELS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/models")
    # Placed beside the executables, which is the first location
    # argus::defaultModelsDir() looks in.
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/models/"
            DESTINATION models
            FILES_MATCHING PATTERN "*.onnx")
endif()

# --- Qt's own deployment tool -----------------------------------------------
# Located through Qt6::qmake rather than PATH, so the deployment always comes
# from the same Qt the build linked against.
get_target_property(_argus_qmake Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_argus_qt_bin "${_argus_qmake}" DIRECTORY)
find_program(ARGUS_DEPLOYQT
    NAMES windeployqt macdeployqt
    HINTS "${_argus_qt_bin}"
    DOC "Qt deployment tool used when packaging")

if(ARGUS_DEPLOYQT)
    if(WIN32)
        # --no-compiler-runtime: windeployqt would otherwise drop an 18 MB
        # vc_redist installer into the package. The runtime DLLs themselves come
        # from InstallRequiredSystemLibraries below, which is both smaller and
        # actually usable without running an installer.
        set(_deploy_args --release --no-translations --no-system-d3d-compiler
                         --no-opengl-sw --no-compiler-runtime)
        set(_deploy_targets "argus-gui.exe" "argus.exe")
    elseif(APPLE)
        set(_deploy_args "")
        set(_deploy_targets "argus-gui.app")
    else()
        set(_deploy_args "")
        set(_deploy_targets "")
    endif()

    foreach(_exe IN LISTS _deploy_targets)
        install(CODE "
            set(_binary \"\${CMAKE_INSTALL_PREFIX}/${_exe}\")
            if(EXISTS \"\${_binary}\")
                message(STATUS \"Deploying Qt for ${_exe}\")
                execute_process(
                    COMMAND \"${ARGUS_DEPLOYQT}\" ${_deploy_args} \"\${_binary}\"
                    RESULT_VARIABLE _deploy_result
                    OUTPUT_QUIET)
                if(NOT _deploy_result EQUAL 0)
                    message(WARNING \"${ARGUS_DEPLOYQT} failed for ${_exe}: \${_deploy_result}\")
                endif()
            endif()
        ")
    endforeach()
else()
    message(STATUS "  deploy tool     : not found — packages will need Qt on the system")
endif()

# macdeployqt fixes application bundles and nothing else, so the headless
# `argus` came out of the package still pointing at /opt/homebrew for Qt and
# every OpenCV module — paths that exist on this machine and on no customer's.
# The bundle was fine and the CLI beside it was unrunnable, which is the kind of
# difference that is only ever found by trying it somewhere else.
#
# fixup_bundle copies each non-system dependency next to the executable and
# rewrites the load commands to @executable_path. It runs last, because it can
# only fix what has already been installed.
if(APPLE)
    # macdeployqt fixes application bundles and nothing else, so the headless
    # `argus` came out of the package still pointing at /opt/homebrew for Qt and
    # every OpenCV module — paths that exist on this machine and on no
    # customer's. The bundle was fine and the CLI beside it was unrunnable.
    #
    # Two approaches were tried and abandoned. BundleUtilities' fixup_bundle
    # assumes a .app layout, computes Frameworks/ one directory too high for a
    # bare executable, and aborts the whole package when one transitive @rpath
    # does not resolve. Repointing the CLI at the bundle's Frameworks got
    # further, then failed too: macdeployqt writes the bundled libraries'
    # own references as @executable_path/../Frameworks/…, which is correct only
    # for an executable inside Contents/MacOS. Homebrew's OpenCV 5 drags in VTK,
    # so that is 276 libraries to rewrite.
    #
    # So the CLI moves to where those paths are already true. The real binary
    # goes inside the bundle next to the GUI, and what sits at the package root
    # is a two-line launcher. Nothing is rewritten and nothing is duplicated.
    install(PROGRAMS "$<TARGET_FILE:argus>"
            DESTINATION "argus-gui.app/Contents/MacOS")

    install(CODE [[
        set(_launcher "${CMAKE_INSTALL_PREFIX}/argus")
        set(_real "${CMAKE_INSTALL_PREFIX}/argus-gui.app/Contents/MacOS/argus")
        if(EXISTS "${_real}")
            message(STATUS "Replacing the CLI with a launcher into the bundle")
            file(WRITE "${_launcher}"
"#!/bin/sh
# The real binary lives inside the application bundle, where the library paths
# macdeployqt wrote are correct. This only forwards to it.
exec \"$(dirname \"$0\")/argus-gui.app/Contents/MacOS/argus\" \"$@\"
")
            execute_process(COMMAND chmod +x "${_launcher}")
        endif()
    ]])
endif()

# ---------------------------------------------------------------------------
# API documentation.
#
# Not part of ALL: generating it on every build would slow the loop down for
# something nobody reads on every build. It is also not required to be present -
# a checkout without Doxygen still builds the tool, and says where to get it.
# ---------------------------------------------------------------------------
find_package(Doxygen QUIET)
if(NOT DOXYGEN_FOUND)
    # tools/docs.* fetch a portable copy here rather than installing anything.
    find_program(ARGUS_DOXYGEN doxygen
        PATHS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/doxygen"
              "${CMAKE_CURRENT_SOURCE_DIR}/third_party/doxygen/bin"
        NO_DEFAULT_PATH)
else()
    set(ARGUS_DOXYGEN "${DOXYGEN_EXECUTABLE}")
endif()

if(ARGUS_DOXYGEN)
    set(ARGUS_DOC_DIR "${CMAKE_BINARY_DIR}/doc")

    # A two-line file that includes the real one and redirects the output, so
    # that the checked-in Doxyfile stays the single description of the docs and
    # running `doxygen` by hand still works without any environment set up.
    file(WRITE "${CMAKE_BINARY_DIR}/Doxyfile.build"
         "@INCLUDE = ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile\n"
         "OUTPUT_DIRECTORY = ${ARGUS_DOC_DIR}\n")

    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ARGUS_DOC_DIR}"
        COMMAND "${ARGUS_DOXYGEN}" "${CMAKE_BINARY_DIR}/Doxyfile.build"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating API documentation into ${ARGUS_DOC_DIR}/html"
        VERBATIM)
    message(STATUS "  docs target     : ${ARGUS_DOXYGEN}")
else()
    message(STATUS "  docs target     : Doxygen not found — run tools/docs.ps1 or tools/docs.sh")
endif()

set(CPACK_PACKAGE_NAME "Argus")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "Argus-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Argus")
set(CPACK_VERBATIM_VARIABLES ON)
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
else()
    set(CPACK_GENERATOR "TGZ")
endif()
include(CPack)

message(STATUS "")
message(STATUS "Argus ${PROJECT_VERSION}")
message(STATUS "  Qt              : ${Qt6_VERSION} (${Qt6_DIR})")
message(STATUS "  build type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "  CLI             : ${ARGUS_BUILD_CLI}")
message(STATUS "  GUI             : ${ARGUS_BUILD_GUI}")
message(STATUS "  inference (P2)  : ${ARGUS_WITH_INFERENCE}")
message(STATUS "")
