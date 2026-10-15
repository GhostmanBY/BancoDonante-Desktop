#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include "models/enums/AppointmentState.hpp"
#include "models/enums/AppointmentType.hpp"

#include <QDateTime>
#include <QJsonObject>
#include <QString>

class Appointment
{
public:
    Appointment() = default;

    static Appointment fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    QDateTime appointmentDatetime() const { return m_appointmentDatetime; }
    AppointmentState state() const { return m_state; }
    AppointmentType type() const { return m_type; }
    int patientId() const { return m_patientId; }
    QString observation() const { return m_observation; }

private:
    int m_id = 0;
    QDateTime m_appointmentDatetime;
    AppointmentState m_state = AppointmentState::PENDING;
    AppointmentType m_type = AppointmentType::CHECK;
    int m_patientId = 0;
    QString m_observation;
};

#endif
