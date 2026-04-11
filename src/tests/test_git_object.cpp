#include <QtTest/QtTest>
#include "../core/GitObject.h"

class TestGitObject : public QObject
{
    Q_OBJECT

private slots:
    void test_dummy()
    {
        QVERIFY(true); // Placeholder
    }
};

QTEST_MAIN(TestGitObject)
#include "test_git_object.moc"
