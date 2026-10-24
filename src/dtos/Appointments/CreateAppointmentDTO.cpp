#include "dtos/Appointments/CreateAppointmentDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

CreateAppointmentDTO::CreateAppointmentDTO(
    QDateTime appointmentDatetime,
    AppointmentType type,
    int patientId,
    QString observation,
    std::optional<int> typeDonationId,
    QString seriesAction)
    : appointmentDatetime(std::move(appointmentDatetime)),
      type(type),
      patientId(patientId),
      observation(std::move(observation)),
      typeDonationId(typeDonationId),
      seriesAction(std::move(seriesAction))
{
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
                appointmentDatetime.toUTC().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    json.insert(QStringLiteral("type"), ::toString(type));
    json.insert(QStringLiteral("patient_id"), patientId);
    json.insert(QStringLiteral("observation"), observation);
    json.insert(
        QStringLiteral("type_donation_id"),
        typeDonationId.has_value() ? *typeDonationId : QJsonValue()
    );
    if (!seriesAction.isEmpty()) {
        json.insert(QStringLiteral("series_action"), seriesAction);
    }
    return json;
}
