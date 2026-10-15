#include "dtos/RestraintXAppointment/CreateRxADTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

CreateRxADTO::CreateRxADTO(int restraintId, int appointmentId)
    : restraintId(restraintId), appointmentId(appointmentId)
{
    if (this->restraintId < 0) {
        throw NegativeNumberException(QStringLiteral("id de la restricion"));
    }

    if (this->appointmentId < 0) {
        throw NegativeNumberException(QStringLiteral("id del turno"));
    }
}

QJsonObject CreateRxADTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("restraint_id"), restraintId);
    json.insert(QStringLiteral("appointment_id"), appointmentId);
    return json;
}
