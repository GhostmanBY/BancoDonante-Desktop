#ifndef CREATEDONATIONDTO_H
#define CREATEDONATIONDTO_H

#include <QJsonObject>
#include <QString>
#include <optional>

class CreateDonationDTO
{
public:
    CreateDonationDTO(
        int appointmentId,
        int typeDonationId,
        std::optional<QString> seriesAction = std::nullopt);

    QJsonObject toJson() const;

    int appointmentId;
    int typeDonationId;
    std::optional<QString> seriesAction;
};

#endif
