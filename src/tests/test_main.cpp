#include <QCoreApplication>
#include <QtTest/QtTest>

#include "test_git_repository.cpp"
#include "test_git_object.cpp"
#include "test_git_commit.cpp"
#include "test_git_index.cpp"
#include "test_git_command_commit.cpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int status = 0;

    // Tests ausführen
    {
        status |= QTest::qExec(new TestGitRepository, argc, argv);
        status |= QTest::qExec(new TestGitObject, argc, argv);
        status |= QTest::qExec(new TestGitCommit, argc, argv);
        status |= QTest::qExec(new TestGitIndex, argc, argv);
        status |= QTest::qExec(new TestGitCommandCommit, argc, argv);
    }

    return status;
}
