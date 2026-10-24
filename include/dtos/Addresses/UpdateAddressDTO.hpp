#ifndef UPDATEADDRESSDTO_H
#define UPDATEADDRESSDTO_H

#include "dtos/Addresses/CreateAddressDTO.hpp"

class UpdateAddressDTO : public CreateAddressDTO
{
public:
    UpdateAddressDTO(
        int id,
        int personId,
        QString street,
        std::optional<QString> streetNumber = std::nullopt,
        std::optional<QString> neighborhood = std::nullopt,
        std::optional<QString> city = std::nullopt,
        std::optional<QString> postalCode = std::nullopt,
        std::optional<QString> floor = std::nullopt,
        std::optional<QString> apartment = std::nullopt,
        std::optional<QString> betweenStreets = std::nullopt,
        bool isCurrent = true);

    QJsonObject toJson() const;

    int id;
};

#endif
