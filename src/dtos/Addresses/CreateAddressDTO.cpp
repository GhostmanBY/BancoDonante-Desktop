#include "dtos/Addresses/CreateAddressDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

CreateAddressDTO::CreateAddressDTO(
    int personId,
    QString street,
    std::optional<QString> streetNumber,
    std::optional<QString> neighborhood,
    std::optional<QString> city,
    std::optional<QString> postalCode,
    std::optional<QString> floor,
    std::optional<QString> apartment,
    std::optional<QString> betweenStreets,
    bool isCurrent)
    : personId(personId),
      street(std::move(street)),
      streetNumber(std::move(streetNumber)),
      neighborhood(std::move(neighborhood)),
      city(std::move(city)),
      postalCode(std::move(postalCode)),
      floor(std::move(floor)),
      apartment(std::move(apartment)),
      betweenStreets(std::move(betweenStreets)),
      isCurrent(isCurrent)
{
    if (this->personId < 0) {
        throw NegativeNumberException(QStringLiteral("id de la persona"));
    }

    if (this->street.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("street"));
    }
    if (this->street.size() > 150) {
        throw InvalidValueException(QStringLiteral("street"));
    }

    if (this->streetNumber.has_value() && this->streetNumber->size() > 20) {
        throw InvalidValueException(QStringLiteral("street_number"));
    }
    if (this->neighborhood.has_value() && this->neighborhood->size() > 100) {
        throw InvalidValueException(QStringLiteral("neighborhood"));
    }
    if (this->city.has_value() && this->city->size() > 100) {
        throw InvalidValueException(QStringLiteral("city"));
    }
    if (this->postalCode.has_value() && this->postalCode->size() > 15) {
        throw InvalidValueException(QStringLiteral("postal_code"));
    }
    if (this->floor.has_value() && this->floor->size() > 10) {
        throw InvalidValueException(QStringLiteral("floor"));
    }
    if (this->apartment.has_value() && this->apartment->size() > 10) {
        throw InvalidValueException(QStringLiteral("apartment"));
    }
    if (this->betweenStreets.has_value() && this->betweenStreets->size() > 255) {
        throw InvalidValueException(QStringLiteral("between_streets"));
    }
}

QJsonObject CreateAddressDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("person_id"), personId);
    json.insert(QStringLiteral("street"), street);
    json.insert(QStringLiteral("street_number"),
                streetNumber.has_value() ? *streetNumber : QJsonValue());
    json.insert(QStringLiteral("neighborhood"),
                neighborhood.has_value() ? *neighborhood : QJsonValue());
    json.insert(QStringLiteral("city"), city.has_value() ? *city : QJsonValue());
    json.insert(QStringLiteral("postal_code"),
                postalCode.has_value() ? *postalCode : QJsonValue());
    json.insert(QStringLiteral("floor"), floor.has_value() ? *floor : QJsonValue());
    json.insert(QStringLiteral("apartment"), apartment.has_value() ? *apartment : QJsonValue());
    json.insert(QStringLiteral("between_streets"),
                betweenStreets.has_value() ? *betweenStreets : QJsonValue());
    json.insert(QStringLiteral("is_current"), isCurrent);
    return json;
}
