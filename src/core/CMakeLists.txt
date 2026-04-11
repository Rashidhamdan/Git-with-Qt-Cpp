find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(core
    GitObject.cpp GitObject.h
    GitBlob.cpp GitBlob.h
    GitTree.cpp GitTree.h
    GitCommit.cpp GitCommit.h
    GitTag.cpp GitTag.h
    GitRepository.cpp GitRepository.h
    GitIndex.cpp GitIndex.h
    GitRepositoryInfo.h GitRepositoryInfo.cpp
    GitExecutable.h GitExecutable.cpp
)

target_link_libraries(core
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
)
target_link_libraries(core PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(core PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
