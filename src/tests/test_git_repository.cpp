#include <QtTest/QtTest>
#include "../core/GitRepository.h"

class TestGitRepository : public QObject
{
    Q_OBJECT

private slots:
    void test_dummy()
    {
        QVERIFY(true); // Placeholder
    }
};

QTEST_MAIN(TestGitRepository)
#include "test_git_repository.moc"
