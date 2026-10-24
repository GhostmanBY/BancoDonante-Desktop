#ifndef CREATEADDRESSDTO_H
#define CREATEADDRESSDTO_H

#include <QJsonObject>
#include <QString>
#include <optional>

class CreateAddressDTO
{
public:
    CreateAddressDTO(
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

    int personId;
    QString street;
    std::optional<QString> streetNumber;
    std::optional<QString> neighborhood;
    std::optional<QString> city;
    std::optional<QString> postalCode;
    std::optional<QString> floor;
    std::optional<QString> apartment;
    std::optional<QString> betweenStreets;
    bool isCurrent;
};

#endif
