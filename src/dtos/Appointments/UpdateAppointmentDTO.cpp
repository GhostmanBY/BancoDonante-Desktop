#include "dtos/Appointments/UpdateAppointmentDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateAppointmentDTO::UpdateAppointmentDTO(
    int id,
    QDateTime appointmentDatetime,
    AppointmentState state,
    AppointmentType type,
    int patientId,
    QString observation,
    QJsonObject clinicalData)
    : CreateAppointmentDTO(
          std::move(appointmentDatetime),
          type,
          patientId,
          std::move(observation)),
      id(id),
      state(state),
      m_clinicalData(std::move(clinicalData))
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
    if (!m_clinicalData.isEmpty()) {
        json.insert(QStringLiteral("clinical_data"), m_clinicalData);
    }
    return json;
}
