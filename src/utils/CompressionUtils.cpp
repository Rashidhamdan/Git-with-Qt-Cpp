#include "CompressionUtils.h"
#include <QByteArray>
#include <QDebug>

QByteArray CompressionUtils::compress(const QByteArray &data) {
    qDebug() << "Dummy compression";
    return data;  // Noch keine echte Kompression
}

QByteArray CompressionUtils::decompress(const QByteArray &data) {
    qDebug() << "Dummy decompression";
    return data;  // Noch keine echte Dekompression
}
