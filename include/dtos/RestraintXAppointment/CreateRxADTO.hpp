#ifndef CREATERXADTO_H
#define CREATERXADTO_H

#include <QJsonObject>

class CreateRxADTO
{
public:
    CreateRxADTO(int restraintId, int appointmentId);

    virtual QJsonObject toJson() const;

    int restraintId;
    int appointmentId;
};

#endif
