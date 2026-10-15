#ifndef UPDATEPERSONESDTO_H
#define UPDATEPERSONESDTO_H

#include "dtos/Persones/CreatePersonesDTO.hpp"

class UpdatePersonesDTO : public CreatePersonesDTO
{
public:
    UpdatePersonesDTO(
        int id,
        QString name,
        QString lastname,
        QString mail,
        QString phone,
        QString documentType,
        QString documentNumber,
        std::optional<QString> birthDate = std::nullopt,
        std::optional<QString> birthPlace = std::nullopt);

    QJsonObject toJson() const override;

    int id;
};

#endif
