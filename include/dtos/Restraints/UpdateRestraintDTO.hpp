#ifndef UPDATERESTRAINTDTO_H
#define UPDATERESTRAINTDTO_H

#include "dtos/Restraints/CreateRestraintDTO.hpp"

class UpdateRestraintDTO : public CreateRestraintDTO
{
public:
    UpdateRestraintDTO(
        int id,
        QString motivo,
        QDateTime estimatedStartDate,
        QDateTime estimatedEndDate);

    QJsonObject toJson() const override;

    int id;
};

#endif
