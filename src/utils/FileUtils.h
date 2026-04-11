#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils {
public:
    static bool fileExists(const QString &path);
};

#endif // FILEUTILS_H
