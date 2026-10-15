#ifndef APPOINTMENTSTATE_H
#define APPOINTMENTSTATE_H

#include <QString>

enum class AppointmentState
{
    PENDING,
    CONFIRMED,
    COMPLETED,
    CANCELLED,
    RESCHEDULED,
    NO_SHOW,
};

inline QString toString(AppointmentState value)
{
    switch (value) {
    case AppointmentState::PENDING: return QStringLiteral("pending");
    case AppointmentState::CONFIRMED: return QStringLiteral("confirmed");
    case AppointmentState::COMPLETED: return QStringLiteral("completed");
    case AppointmentState::CANCELLED: return QStringLiteral("cancelled");
    case AppointmentState::RESCHEDULED: return QStringLiteral("rescheduled");
    case AppointmentState::NO_SHOW: return QStringLiteral("no_show");
    }
    return {};
}

inline AppointmentState appointmentStateFromString(const QString &value, AppointmentState fallback)
{
    if (value == QLatin1String("pending")) return AppointmentState::PENDING;
    if (value == QLatin1String("confirmed")) return AppointmentState::CONFIRMED;
    if (value == QLatin1String("completed")) return AppointmentState::COMPLETED;
    if (value == QLatin1String("cancelled")) return AppointmentState::CANCELLED;
    if (value == QLatin1String("rescheduled")) return AppointmentState::RESCHEDULED;
    if (value == QLatin1String("no_show")) return AppointmentState::NO_SHOW;
    return fallback;
}

#endif
