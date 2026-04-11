#include <QtTest/QtTest>
#include "../commands/GitCommandCommit.h"

class TestGitCommandCommit : public QObject
{
    Q_OBJECT

private slots:
    void test_dummy()
    {
        QVERIFY(true); // Placeholder
    }
};

QTEST_MAIN(TestGitCommandCommit)
#include "test_git_command_commit.moc"
