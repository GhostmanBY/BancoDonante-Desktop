#include "models/Restraint.hpp"

#include "core/JsonUtils.hpp"

Restraint Restraint::fromJson(const QJsonObject &json)
{
    Restraint model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_motivo = jsonutils::stringOr(json, QStringLiteral("motivo"));
    model.m_estimatedStartDate = jsonutils::parseDateTime(
        jsonutils::stringOr(json, QStringLiteral("estimated_start_date")));
    model.m_estimatedEndDate = jsonutils::parseDateTime(
        jsonutils::stringOr(json, QStringLiteral("estimated_end_date")));
    model.m_isActive = jsonutils::boolOr(json, QStringLiteral("isActive"), true);
    return model;
}

QJsonObject Restraint::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("motivo"), m_motivo);
    json.insert(QStringLiteral("estimated_start_date"),
                m_estimatedStartDate.toString(QStringLiteral("yyyy-MM-dd")));
    json.insert(QStringLiteral("estimated_end_date"),
                m_estimatedEndDate.toString(QStringLiteral("yyyy-MM-dd")));
    json.insert(QStringLiteral("isActive"), m_isActive);
    return json;
}

QString Restraint::toString() const
{
    return QStringLiteral("Restraint(id=%1, motivo=%2, estimated_start_date=%3, estimated_end_date=%4, isActive=%5)")
        .arg(m_id)
        .arg(m_motivo)
        .arg(m_estimatedStartDate.toString(QStringLiteral("yyyy-MM-dd")))
        .arg(m_estimatedEndDate.toString(QStringLiteral("yyyy-MM-dd")))
        .arg(m_isActive ? QStringLiteral("true") : QStringLiteral("false"));
}
