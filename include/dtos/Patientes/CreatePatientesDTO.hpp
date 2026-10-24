#ifndef CREATEPATIENTESDTO_H
#define CREATEPATIENTESDTO_H

#include "dtos/Persones/CreatePersonesDTO.hpp"
#include "models/enums/BloodGroup.hpp"
#include "models/enums/Sex.hpp"

class CreatePatientesDTO : public CreatePersonesDTO
{
public:
    CreatePatientesDTO(
        QString name,
        QString lastname,
        QString mail,
        QString phone,
        QString documentType,
        QString documentNumber,
        std::optional<QString> birthDate,
        std::optional<QString> birthPlace,
        Sex sex,
        double peso,
        double altura,
        BloodGroup bloodGroup);

    QJsonObject toJson() const override;

    Sex sex;
    double peso;
    double altura;
    BloodGroup bloodGroup;
};

#endif
