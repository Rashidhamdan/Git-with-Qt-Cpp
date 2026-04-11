add_library(commands
    GitCommand.cpp GitCommand.h
    GitCommandInit.cpp GitCommandInit.h
    GitCommandAdd.cpp GitCommandAdd.h
    GitCommandCommit.cpp GitCommandCommit.h
    GitCommandStatus.cpp GitCommandStatus.h
    GitCommandLog.cpp GitCommandLog.h
    GitCommandDiff.cpp GitCommandDiff.h
    GitCommandBranch.cpp GitCommandBranch.h
    GitCommandCheckout.cpp GitCommandCheckout.h
    GitCommandPush.cpp GitCommandPush.h
    GitCommandPull.cpp GitCommandPull.h
)

target_link_libraries(commands
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
    core
    utils
)

target_include_directories(commands PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
