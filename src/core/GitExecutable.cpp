#include "GitExecutable.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

QString GitExecutable::path()
{
    const QString fromPath = QStandardPaths::findExecutable(QStringLiteral("git"));
    if (!fromPath.trimmed().isEmpty()) {
        return QDir::toNativeSeparators(fromPath);
    }

#ifdef Q_OS_WIN
    const QStringList candidates = {
        QStringLiteral("C:/Program Files/Git/cmd/git.exe"),
        QStringLiteral("C:/Program Files/Git/bin/git.exe"),
        QStringLiteral("C:/Program Files (x86)/Git/cmd/git.exe"),
        QStringLiteral("C:/Program Files (x86)/Git/bin/git.exe")
    };

    for (const QString& candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return QDir::toNativeSeparators(candidate);
        }
    }
#endif

    return QString();
}

bool GitExecutable::isAvailable()
{
    return !path().trimmed().isEmpty();
}
