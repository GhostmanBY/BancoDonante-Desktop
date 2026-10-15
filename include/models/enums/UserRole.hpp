#ifndef USERROLE_H
#define USERROLE_H

#include <QString>

enum class UserRole
{
    AREA_MANAGER,
    NURSE,
    MEDICAL,
    ADMIN,
};

inline QString toString(UserRole value)
{
    switch (value) {
    case UserRole::AREA_MANAGER: return QStringLiteral("area_manager");
    case UserRole::NURSE: return QStringLiteral("nurse");
    case UserRole::MEDICAL: return QStringLiteral("medical");
    case UserRole::ADMIN: return QStringLiteral("admin");
    }
    return {};
}

inline UserRole userRoleFromString(const QString &value, UserRole fallback)
{
    if (value == QLatin1String("area_manager")) return UserRole::AREA_MANAGER;
    if (value == QLatin1String("nurse")) return UserRole::NURSE;
    if (value == QLatin1String("medical")) return UserRole::MEDICAL;
    if (value == QLatin1String("admin")) return UserRole::ADMIN;
    return fallback;
}

#endif
