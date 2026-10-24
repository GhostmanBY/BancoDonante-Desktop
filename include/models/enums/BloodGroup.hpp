#ifndef BLOODGROUP_H
#define BLOODGROUP_H

#include <QString>

enum class BloodGroup
{
    A_POS,
    A_NEG,
    B_POS,
    B_NEG,
    AB_POS,
    AB_NEG,
    O_POS,
    O_NEG,
};

inline QString toString(BloodGroup value)
{
    switch (value) {
    case BloodGroup::A_POS: return QStringLiteral("A+");
    case BloodGroup::A_NEG: return QStringLiteral("A-");
    case BloodGroup::B_POS: return QStringLiteral("B+");
    case BloodGroup::B_NEG: return QStringLiteral("B-");
    case BloodGroup::AB_POS: return QStringLiteral("AB+");
    case BloodGroup::AB_NEG: return QStringLiteral("AB-");
    case BloodGroup::O_POS: return QStringLiteral("O+");
    case BloodGroup::O_NEG: return QStringLiteral("O-");
    }
    return {};
}

inline BloodGroup bloodGroupFromString(const QString &value, BloodGroup fallback)
{
    if (value == QLatin1String("A+")) return BloodGroup::A_POS;
    if (value == QLatin1String("A-")) return BloodGroup::A_NEG;
    if (value == QLatin1String("B+")) return BloodGroup::B_POS;
    if (value == QLatin1String("B-")) return BloodGroup::B_NEG;
    if (value == QLatin1String("AB+")) return BloodGroup::AB_POS;
    if (value == QLatin1String("AB-")) return BloodGroup::AB_NEG;
    if (value == QLatin1String("O+")) return BloodGroup::O_POS;
    if (value == QLatin1String("O-")) return BloodGroup::O_NEG;
    return fallback;
}

#endif
