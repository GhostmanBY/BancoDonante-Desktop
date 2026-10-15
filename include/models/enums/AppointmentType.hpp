#ifndef APPOINTMENTTYPE_H
#define APPOINTMENTTYPE_H

#include <QString>

enum class AppointmentType
{
    DONATION,
    CHECK,
};

inline QString toString(AppointmentType value)
{
    switch (value) {
    case AppointmentType::DONATION: return QStringLiteral("donation");
    case AppointmentType::CHECK: return QStringLiteral("check");
    }
    return {};
}

inline AppointmentType appointmentTypeFromString(const QString &value, AppointmentType fallback)
{
    if (value == QLatin1String("donation")) return AppointmentType::DONATION;
    if (value == QLatin1String("check")) return AppointmentType::CHECK;
    return fallback;
}

#endif
