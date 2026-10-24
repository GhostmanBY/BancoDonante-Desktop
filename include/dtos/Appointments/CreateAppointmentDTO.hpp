#ifndef CREATEAPPOINTMENTDTO_H
#define CREATEAPPOINTMENTDTO_H

#include "models/enums/AppointmentType.hpp"

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <optional>

class CreateAppointmentDTO
{
public:
    CreateAppointmentDTO(
        QDateTime appointmentDatetime,
        AppointmentType type,
        int patientId,
        QString observation,
        std::optional<int> typeDonationId = std::nullopt,
        QString seriesAction = QString());

    QJsonObject toJson() const;

    QDateTime appointmentDatetime;
    AppointmentType type;
    int patientId;
    QString observation;
    std::optional<int> typeDonationId;
    QString seriesAction;
};

#endif
