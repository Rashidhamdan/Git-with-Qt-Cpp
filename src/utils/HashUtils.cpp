#include "HashUtils.h"
#include <QCryptographicHash>

QString HashUtils::sha1(const QString &input) {
    QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1);
    return QString(hash.toHex());
}
