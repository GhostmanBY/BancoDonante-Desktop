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

inline QString userRoleLabel(UserRole value)
{
    switch (value) {
    case UserRole::AREA_MANAGER: return QStringLiteral("Responsable de área");
    case UserRole::NURSE: return QStringLiteral("Enfermero");
    case UserRole::MEDICAL: return QStringLiteral("Médico");
    case UserRole::ADMIN: return QStringLiteral("Administrador");
    }
    return {};
}

// Jerarquía: admin > area_manager > medical > nurse (mayor = más poder).
inline int roleRank(UserRole value)
{
    switch (value) {
    case UserRole::ADMIN: return 4;
    case UserRole::AREA_MANAGER: return 3;
    case UserRole::MEDICAL: return 2;
    case UserRole::NURSE: return 1;
    }
    return 0;
}

// True si `higher` está estrictamente por encima de `lower`
// (puede gestionar usuarios de ese rango).
inline bool roleIsAbove(UserRole higher, UserRole lower)
{
    return roleRank(higher) > roleRank(lower);
}

#endif
