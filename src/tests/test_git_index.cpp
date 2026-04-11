#include <QtTest/QtTest>
#include "../core/GitIndex.h"

class TestGitIndex : public QObject
{
    Q_OBJECT

private slots:
    void test_dummy()
    {
        QVERIFY(true); // Placeholder
    }
};

QTEST_MAIN(TestGitIndex)
#include "test_git_index.moc"
