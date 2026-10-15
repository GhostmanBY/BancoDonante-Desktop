#include "dtos/Appointments/CreateAppointmentDTO.hpp"

#include "exceptions/Validation/DateInThePastException.hpp"
#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

CreateAppointmentDTO::CreateAppointmentDTO(
    QDateTime appointmentDatetime,
    AppointmentType type,
    int patientId,
    QString observation)
    : appointmentDatetime(std::move(appointmentDatetime)),
      type(type),
      patientId(patientId),
      observation(std::move(observation))
{
    if (this->appointmentDatetime < QDateTime::currentDateTime()) {
        throw DateInThePastException(QStringLiteral("appointment_datetime"));
    }

    if (this->patientId < 0) {
        throw NegativeNumberException(QStringLiteral("id del paciente"));
    }

    if (this->observation.size() > 255) {
        throw InvalidValueException(QStringLiteral("observation"));
    }
}

QJsonObject CreateAppointmentDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("appointment_datetime"),
                appointmentDatetime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    json.insert(QStringLiteral("type"), ::toString(type));
    json.insert(QStringLiteral("patient_id"), patientId);
    json.insert(QStringLiteral("observation"), observation);
    return json;
}
