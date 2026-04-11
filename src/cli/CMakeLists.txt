add_library(cli
    CommandParser.cpp CommandParser.h
    CommandRouter.cpp CommandRouter.h
)

target_link_libraries(cli
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
    commands
)

target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
