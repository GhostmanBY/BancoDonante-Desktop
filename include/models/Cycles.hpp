#ifndef CYCLES_H
#define CYCLES_H

#include "models/enums/CycleState.hpp"

#include <QDateTime>
#include <QJsonObject>
#include <optional>

class Cycles
{
public:
    Cycles() = default;

    static Cycles fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    int patientId() const { return m_patientId; }
    int cycleRuleId() const { return m_cycleRuleId; }
    CycleState state() const { return m_state; }
    QDateTime openedAt() const { return m_openedAt; }
    std::optional<QDateTime> closedAt() const { return m_closedAt; }
    int donationsCount() const { return m_donationsCount; }

private:
    int m_id = 0;
    int m_patientId = 0;
    int m_cycleRuleId = 0;
    CycleState m_state = CycleState::CLOSED;
    QDateTime m_openedAt;
    std::optional<QDateTime> m_closedAt;
    int m_donationsCount = 0;
};

#endif
