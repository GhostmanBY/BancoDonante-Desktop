#ifndef UPDATERXADTO_H
#define UPDATERXADTO_H

#include "dtos/RestraintXAppointment/CreateRxADTO.hpp"

class UpdateRxADTO : public CreateRxADTO
{
public:
    UpdateRxADTO(int id, int restraintId, int appointmentId);

    QJsonObject toJson() const override;

    int id;
};

#endif
