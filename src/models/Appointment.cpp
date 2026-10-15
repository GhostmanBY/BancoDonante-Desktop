#include "models/Appointment.hpp"

#include "core/JsonUtils.hpp"

Appointment Appointment::fromJson(const QJsonObject &json)
{
    Appointment model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_appointmentDatetime = jsonutils::parseDateTime(
        jsonutils::stringOr(json, QStringLiteral("appointment_datetime")));
    model.m_state = appointmentStateFromString(
        jsonutils::stringOr(json, QStringLiteral("state")),
        AppointmentState::PENDING);
    model.m_type = appointmentTypeFromString(
        jsonutils::stringOr(json, QStringLiteral("type")),
        AppointmentType::CHECK);
    model.m_patientId = jsonutils::intOr(json, QStringLiteral("patient_id"));
    model.m_observation = jsonutils::stringOr(json, QStringLiteral("observation"));
    return model;
}

QJsonObject Appointment::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("appointment_datetime"), m_appointmentDatetime.toString(Qt::ISODate));
    json.insert(QStringLiteral("state"), ::toString(m_state));
    json.insert(QStringLiteral("type"), ::toString(m_type));
    json.insert(QStringLiteral("patient_id"), m_patientId);
    json.insert(QStringLiteral("observation"), m_observation);
    return json;
}

QString Appointment::toString() const
{
    return QStringLiteral("Appointment(id=%1, appointment_datetime=%2, state=%3, type=%4, patient_id=%5, observation=%6)")
        .arg(m_id)
        .arg(m_appointmentDatetime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")))
        .arg(::toString(m_state), ::toString(m_type))
        .arg(m_patientId)
        .arg(m_observation);
}
