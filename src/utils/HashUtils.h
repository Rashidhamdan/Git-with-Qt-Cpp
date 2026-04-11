#ifndef HASHUTILS_H
#define HASHUTILS_H

#include <QString>

class HashUtils {
public:
    static QString sha1(const QString &input);
};

#endif // HASHUTILS_H
