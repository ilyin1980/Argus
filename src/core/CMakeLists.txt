qt_add_library(imageworker_core STATIC
    Types.h
    Parallel.h
    Paths.h              Paths.cpp
    ContentHash.h        ContentHash.cpp
    PerceptualHash.h     PerceptualHash.cpp
    ImageDecoder.h       ImageDecoder.cpp
    Scanner.h            Scanner.cpp
    Database.h           Database.cpp
    Indexer.h            Indexer.cpp
    DuplicateFinder.h    DuplicateFinder.cpp
    QueryEngine.h        QueryEngine.cpp
    Embedder.h           Embedder.cpp
    Onnx.h               Onnx.cpp
    OnnxProvider.h       OnnxProvider.cpp
    FeatureExtractor.h   FeatureExtractor.cpp
    DescriptorStore.h    DescriptorStore.cpp
    FeatureMatcher.h     FeatureMatcher.cpp
    TemplateMatcher.h    TemplateMatcher.cpp
    Vocabulary.h         Vocabulary.cpp
    BowIndex.h           BowIndex.cpp
    ObjectFinder.h       ObjectFinder.cpp
    Json.h               Json.cpp
)

target_include_directories(imageworker_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries(imageworker_core
    PUBLIC Qt6::Core Qt6::Gui Qt6::Sql)

target_compile_definitions(imageworker_core
    PUBLIC IMAGEWORKER_VERSION="${PROJECT_VERSION}")

if(IMAGEWORKER_WITH_INFERENCE)
    target_compile_definitions(imageworker_core PUBLIC IMAGEWORKER_WITH_INFERENCE=1)
    target_link_libraries(imageworker_core PUBLIC onnxruntime ${OpenCV_LIBS})
    target_include_directories(imageworker_core PUBLIC ${OpenCV_INCLUDE_DIRS})
endif()

if(MSVC)
    target_compile_options(imageworker_core PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(imageworker_core PRIVATE -Wall -Wextra)
endif()
