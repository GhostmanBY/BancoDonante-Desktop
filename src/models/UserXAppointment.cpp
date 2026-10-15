#include "models/UserXAppointment.hpp"

#include "core/JsonUtils.hpp"

UserXAppointment UserXAppointment::fromJson(const QJsonObject &json)
{
    UserXAppointment model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_userId = jsonutils::intOr(json, QStringLiteral("user_id"));
    model.m_appointmentId = jsonutils::intOr(json, QStringLiteral("appointment_id"));
    return model;
}

QJsonObject UserXAppointment::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("user_id"), m_userId);
    json.insert(QStringLiteral("appointment_id"), m_appointmentId);
    return json;
}

QString UserXAppointment::toString() const
{
    return QStringLiteral("UserXAppointment(id=%1, user_id=%2, appointment_id=%3)")
        .arg(m_id)
        .arg(m_userId)
        .arg(m_appointmentId);
}
