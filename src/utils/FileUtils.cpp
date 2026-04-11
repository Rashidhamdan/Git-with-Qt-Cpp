#include "FileUtils.h"
#include <QFile>

bool FileUtils::fileExists(const QString &path) {
    return QFile::exists(path);
}
