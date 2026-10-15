#ifndef SEX_H
#define SEX_H

#include <QString>

enum class Sex
{
    M,
    F,
};

inline QString toString(Sex value)
{
    switch (value) {
    case Sex::M: return QStringLiteral("M");
    case Sex::F: return QStringLiteral("F");
    }
    return {};
}

inline Sex sexFromString(const QString &value, Sex fallback)
{
    if (value == QLatin1String("M")) return Sex::M;
    if (value == QLatin1String("F")) return Sex::F;
    return fallback;
}

#endif
