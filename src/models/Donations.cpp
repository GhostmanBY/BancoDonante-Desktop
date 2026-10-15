#include "models/Donations.hpp"

#include "core/JsonUtils.hpp"

Donations Donations::fromJson(const QJsonObject &json)
{
    Donations model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_appointmentId = jsonutils::intOr(json, QStringLiteral("appointment_id"));
    model.m_typeDonationId = jsonutils::intOr(json, QStringLiteral("type_donation_id"));
    model.m_cycleId = jsonutils::intOr(json, QStringLiteral("cycle_id"));
    return model;
}

QJsonObject Donations::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("appointment_id"), m_appointmentId);
    json.insert(QStringLiteral("type_donation_id"), m_typeDonationId);
    json.insert(QStringLiteral("cycle_id"), m_cycleId);
    return json;
}

QString Donations::toString() const
{
    return QStringLiteral("Donations(id=%1, appointment_id=%2, type_donation_id=%3, cycle_id=%4)")
        .arg(m_id)
        .arg(m_appointmentId)
        .arg(m_typeDonationId)
        .arg(m_cycleId);
}
