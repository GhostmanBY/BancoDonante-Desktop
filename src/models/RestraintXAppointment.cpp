#include "models/RestraintXAppointment.hpp"

#include "core/JsonUtils.hpp"

RestraintXAppointment RestraintXAppointment::fromJson(const QJsonObject &json)
{
    RestraintXAppointment model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_restraintId = jsonutils::intOr(json, QStringLiteral("restraint_id"));
    model.m_appointmentId = jsonutils::intOr(json, QStringLiteral("appointment_id"));
    return model;
}

QJsonObject RestraintXAppointment::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("restraint_id"), m_restraintId);
    json.insert(QStringLiteral("appointment_id"), m_appointmentId);
    return json;
}

QString RestraintXAppointment::toString() const
{
    return QStringLiteral("RestraintXAppointment(id=%1, restraint_id=%2, appointment_id=%3)")
        .arg(m_id)
        .arg(m_restraintId)
        .arg(m_appointmentId);
}
