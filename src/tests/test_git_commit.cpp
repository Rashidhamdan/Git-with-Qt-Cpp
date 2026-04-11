#include <QtTest/QtTest>
#include "../core/GitCommit.h"

class TestGitCommit : public QObject
{
    Q_OBJECT

private slots:
    void test_dummy()
    {
        QVERIFY(true); // Placeholder
    }
};

QTEST_MAIN(TestGitCommit)
#include "test_git_commit.moc"
