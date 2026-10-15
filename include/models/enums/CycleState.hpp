#ifndef CYCLESTATE_H
#define CYCLESTATE_H

#include <QString>

enum class CycleState
{
    OPEN,
    CLOSED,
};

inline QString toString(CycleState value)
{
    switch (value) {
    case CycleState::OPEN: return QStringLiteral("open");
    case CycleState::CLOSED: return QStringLiteral("closed");
    }
    return {};
}

inline CycleState cycleStateFromString(const QString &value, CycleState fallback)
{
    if (value == QLatin1String("open")) return CycleState::OPEN;
    if (value == QLatin1String("closed")) return CycleState::CLOSED;
    return fallback;
}

#endif
