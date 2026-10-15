#include "dtos/Donations/CreateDonationDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

CreateDonationDTO::CreateDonationDTO(
    int appointmentId,
    int typeDonationId,
    std::optional<QString> seriesAction)
    : appointmentId(appointmentId),
      typeDonationId(typeDonationId),
      seriesAction(std::move(seriesAction))
{
    if (this->appointmentId < 0) {
        throw NegativeNumberException(QStringLiteral("id del turno"));
    }

    if (this->typeDonationId < 0) {
        throw NegativeNumberException(QStringLiteral("id del tipo de donación"));
    }

    if (this->seriesAction.has_value()
        && *this->seriesAction != QLatin1String("continue")
        && *this->seriesAction != QLatin1String("new")) {
        throw InvalidValueException(QStringLiteral("series_action"));
    }
}

QJsonObject CreateDonationDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("appointment_id"), appointmentId);
    json.insert(QStringLiteral("type_donation_id"), typeDonationId);
    json.insert(QStringLiteral("series_action"),
                seriesAction.has_value() ? *seriesAction : QJsonValue());
    return json;
}
