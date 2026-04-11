#include "Logger.h"
#include <QDebug>

void Logger::log(const QString &message) {
    qDebug() << "[LOG]:" << message;
}
