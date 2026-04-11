enable_testing()

add_executable(git_tests
    test_main.cpp
    test_git_repository.cpp
    test_git_object.cpp
    test_git_commit.cpp
    test_git_index.cpp
    test_git_command_commit.cpp
)

target_link_libraries(git_tests
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
        core
        commands
        utils
)

add_test(NAME GitTests COMMAND git_tests)
