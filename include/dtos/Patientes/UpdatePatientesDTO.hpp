#ifndef UPDATEPATIENTESDTO_H
#define UPDATEPATIENTESDTO_H

#include "dtos/Persones/UpdatePersonesDTO.hpp"
#include "models/enums/PatientState.hpp"
#include "models/enums/Sex.hpp"

class UpdatePatientesDTO : public UpdatePersonesDTO
{
public:
    UpdatePatientesDTO(
        int id,
        QString name,
        QString lastname,
        QString mail,
        QString phone,
        QString documentType,
        QString documentNumber,
        std::optional<QString> birthDate,
        std::optional<QString> birthPlace,
        PatientState stateDonation,
        Sex sex,
        double peso,
        double altura);

    QJsonObject toJson() const override;

    PatientState stateDonation;
    Sex sex;
    double peso;
    double altura;
};

#endif
