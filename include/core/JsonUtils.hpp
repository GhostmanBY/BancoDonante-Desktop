#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <optional>

namespace jsonutils {

inline std::optional<QString> optString(const QJsonObject &obj, const QString &key)
{
    const QJsonValue value = obj.value(key);
    if (value.isNull() || !value.isString()) {
        return std::nullopt;
    }
    return value.toString();
}

inline QString stringOr(const QJsonObject &obj, const QString &key, const QString &fallback = QString())
{
    return optString(obj, key).value_or(fallback);
}

inline int intOr(const QJsonObject &obj, const QString &key, int fallback = 0)
{
    const QJsonValue value = obj.value(key);
    if (value.isDouble()) {
        return value.toInt(fallback);
    }
    return fallback;
}

inline bool boolOr(const QJsonObject &obj, const QString &key, bool fallback = false)
{
    const QJsonValue value = obj.value(key);
    if (value.isBool()) {
        return value.toBool();
    }
    return fallback;
}

inline std::optional<double> optDouble(const QJsonObject &obj, const QString &key)
{
    const QJsonValue value = obj.value(key);
    if (value.isNull() || !value.isDouble()) {
        return std::nullopt;
    }
    return value.toDouble();
}

inline QDateTime parseDateTime(const QString &raw)
{
    if (raw.isEmpty()) {
        return {};
    }

    QDateTime dateTime = QDateTime::fromString(raw, Qt::ISODate);
    if (dateTime.isValid()) {
        return dateTime;
    }

    dateTime = QDateTime::fromString(raw, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (dateTime.isValid()) {
        return dateTime;
    }

    dateTime = QDateTime::fromString(raw, QStringLiteral("yyyy-MM-dd"));
    return dateTime;
}

} // namespace jsonutils

#endif
