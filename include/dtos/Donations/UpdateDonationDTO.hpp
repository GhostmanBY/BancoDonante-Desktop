#ifndef UPDATEDONATIONDTO_H
#define UPDATEDONATIONDTO_H

#include <QJsonObject>

class UpdateDonationDTO
{
public:
    UpdateDonationDTO(int id, int appointmentId, int typeDonationId);

    QJsonObject toJson() const;

    int id;
    int appointmentId;
    int typeDonationId;
};

#endif
