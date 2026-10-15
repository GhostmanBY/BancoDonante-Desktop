#include "dtos/Addresses/UpdateAddressDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateAddressDTO::UpdateAddressDTO(
    int id,
    int personId,
    QString street,
    std::optional<QString> streetNumber,
    std::optional<QString> neighborhood,
    std::optional<QString> city,
    std::optional<QString> postalCode,
    std::optional<QString> country,
    std::optional<QString> floor,
    std::optional<QString> apartment,
    std::optional<QString> betweenStreets,
    bool isCurrent)
    : CreateAddressDTO(
          personId,
          std::move(street),
          std::move(streetNumber),
          std::move(neighborhood),
          std::move(city),
          std::move(postalCode),
          std::move(country),
          std::move(floor),
          std::move(apartment),
          std::move(betweenStreets),
          isCurrent),
      id(id)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la dirección"));
    }
}

QJsonObject UpdateAddressDTO::toJson() const
{
    QJsonObject json = CreateAddressDTO::toJson();
    json.insert(QStringLiteral("id"), id);
    return json;
}
