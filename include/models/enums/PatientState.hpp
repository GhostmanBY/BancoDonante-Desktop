#ifndef PATIENTSTATE_H
#define PATIENTSTATE_H

#include <QString>

enum class PatientState
{
    APTO,
    REVISON,
    NO_APTO,
    SUSPENDED,
};

inline QString toString(PatientState value)
{
    switch (value) {
    case PatientState::APTO: return QStringLiteral("apto");
    case PatientState::REVISON: return QStringLiteral("revision");
    case PatientState::NO_APTO: return QStringLiteral("no-apto");
    case PatientState::SUSPENDED: return QStringLiteral("suspended");
    }
    return {};
}

inline PatientState patientStateFromString(const QString &value, PatientState fallback)
{
    if (value == QLatin1String("apto")) return PatientState::APTO;
    if (value == QLatin1String("revision")) return PatientState::REVISON;
    if (value == QLatin1String("no-apto")) return PatientState::NO_APTO;
    if (value == QLatin1String("suspended")) return PatientState::SUSPENDED;
    return fallback;
}

#endif
