#include "dtos/Appointments/UpdateAppointmentDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateAppointmentDTO::UpdateAppointmentDTO(
    int id,
    QDateTime appointmentDatetime,
    AppointmentState state,
    AppointmentType type,
    int patientId,
    QString observation)
    : CreateAppointmentDTO(
          std::move(appointmentDatetime),
          type,
          patientId,
          std::move(observation)),
      id(id),
      state(state)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id del turno"));
    }
}

QJsonObject UpdateAppointmentDTO::toJson() const
{
    QJsonObject json = CreateAppointmentDTO::toJson();
    json.insert(QStringLiteral("id"), id);
    json.insert(QStringLiteral("state"), ::toString(state));
    return json;
}
