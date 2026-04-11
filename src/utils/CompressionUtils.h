#ifndef COMPRESSIONUTILS_H
#define COMPRESSIONUTILS_H

#include <QByteArray>

class CompressionUtils {
public:
    static QByteArray compress(const QByteArray &data);
    static QByteArray decompress(const QByteArray &data);
};

#endif // COMPRESSIONUTILS_H
