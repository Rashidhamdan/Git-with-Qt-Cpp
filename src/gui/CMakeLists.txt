find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(gui
    MainWindow.cpp MainWindow.h MainWindow.ui
    CommitDialog.cpp CommitDialog.h CommitDialog.ui
    InitRepoDialog.cpp InitRepoDialog.h InitRepoDialog.ui
    BranchManagerDialog.cpp BranchManagerDialog.h BranchManagerDialog.ui
    StashDialog.cpp StashDialog.h StashDialog.ui
    DiffViewerWidget.cpp DiffViewerWidget.h DiffViewerWidget.ui
    LogViewerWidget.cpp LogViewerWidget.h LogViewerWidget.ui
    CommandConsoleWidget.cpp CommandConsoleWidget.h CommandConsoleWidget.ui
    RepoExplorerWidget.cpp RepoExplorerWidget.h RepoExplorerWidget.ui
    StatusWidget.cpp StatusWidget.h StatusWidget.ui
    RepoWidget.h RepoWidget.cpp

)

target_link_libraries(gui
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    core
    commands
    cli
    utils
)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
