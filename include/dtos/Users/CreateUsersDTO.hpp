#ifndef CREATEUSERSDTO_H
#define CREATEUSERSDTO_H

#include "dtos/Persones/CreatePersonesDTO.hpp"
#include "models/enums/UserRole.hpp"

class CreateUsersDTO : public CreatePersonesDTO
{
public:
    CreateUsersDTO(
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
