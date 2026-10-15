#include "dtos/Donations/UpdateDonationDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateDonationDTO::UpdateDonationDTO(int id, int appointmentId, int typeDonationId)
    : id(id), appointmentId(appointmentId), typeDonationId(typeDonationId)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la donación"));
    }

    if (this->appointmentId < 0) {
        throw NegativeNumberException(QStringLiteral("id del turno"));
    }

    if (this->typeDonationId < 0) {
        throw NegativeNumberException(QStringLiteral("id del tipo de donación"));
    }
}

QJsonObject UpdateDonationDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), id);
    json.insert(QStringLiteral("appointment_id"), appointmentId);
    json.insert(QStringLiteral("type_donation_id"), typeDonationId);
    return json;
}
