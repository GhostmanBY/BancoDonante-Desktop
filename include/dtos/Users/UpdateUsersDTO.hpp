#ifndef UPDATEUSERSDTO_H
#define UPDATEUSERSDTO_H

#include "dtos/Persones/UpdatePersonesDTO.hpp"
#include "models/enums/UserRole.hpp"

class UpdateUsersDTO : public UpdatePersonesDTO
{
public:
    UpdateUsersDTO(
        int id,
        QString name,
        QString lastname,
        QString mail,
        QString phone,
        QString documentType,
        QString documentNumber,
        std::optional<QString> birthDate,
        std::optional<QString> birthPlace,
        QString username,
        QString hashPassword,
        UserRole role);

    QJsonObject toJson() const override;

    QString username;
    QString hashPassword;
    UserRole role;
};

#endif
