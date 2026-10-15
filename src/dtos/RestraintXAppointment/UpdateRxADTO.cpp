#include "dtos/RestraintXAppointment/UpdateRxADTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateRxADTO::UpdateRxADTO(int id, int restraintId, int appointmentId)
    : CreateRxADTO(restraintId, appointmentId), id(id)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la relacion"));
    }
}

QJsonObject UpdateRxADTO::toJson() const
{
    QJsonObject json = CreateRxADTO::toJson();
    json.insert(QStringLiteral("id"), id);
    return json;
}
