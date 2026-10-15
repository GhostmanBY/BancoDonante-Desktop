#include "models/Cycles.hpp"

#include "core/JsonUtils.hpp"

Cycles Cycles::fromJson(const QJsonObject &json)
{
    Cycles model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_patientId = jsonutils::intOr(json, QStringLiteral("patient_id"));
    model.m_cycleRuleId = jsonutils::intOr(json, QStringLiteral("cycle_rule_id"));
    model.m_state = cycleStateFromString(
        jsonutils::stringOr(json, QStringLiteral("state")),
        CycleState::CLOSED);
    model.m_openedAt = jsonutils::parseDateTime(
        jsonutils::stringOr(json, QStringLiteral("opened_at")));

    const auto closedAt = jsonutils::optString(json, QStringLiteral("closed_at"));
    if (closedAt.has_value()) {
        model.m_closedAt = jsonutils::parseDateTime(*closedAt);
    }

    model.m_donationsCount = jsonutils::intOr(json, QStringLiteral("donations_count"));
    return model;
}

QJsonObject Cycles::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("patient_id"), m_patientId);
    json.insert(QStringLiteral("cycle_rule_id"), m_cycleRuleId);
    json.insert(QStringLiteral("state"), ::toString(m_state));
    json.insert(QStringLiteral("opened_at"),
                m_openedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    json.insert(QStringLiteral("closed_at"),
                m_closedAt.has_value()
                    ? m_closedAt->toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                    : QJsonValue());
    json.insert(QStringLiteral("donations_count"), m_donationsCount);
    return json;
}

QString Cycles::toString() const
{
    return QStringLiteral("Cycles(id=%1, patient_id=%2, cycle_rule_id=%3, state=%4, opened_at=%5, closed_at=%6, donations_count=%7)")
        .arg(m_id)
        .arg(m_patientId)
        .arg(m_cycleRuleId)
        .arg(::toString(m_state))
        .arg(m_openedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")))
        .arg(m_closedAt.has_value() ? m_closedAt->toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                                    : QStringLiteral("null"))
        .arg(m_donationsCount);
}
