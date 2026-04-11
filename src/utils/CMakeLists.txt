add_library(utils
    Logger.cpp Logger.h
    FileUtils.cpp FileUtils.h
    HashUtils.cpp HashUtils.h
    CompressionUtils.cpp CompressionUtils.h
)

target_link_libraries(utils
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
