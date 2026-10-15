#ifndef CREATEAPPOINTMENTDTO_H
#define CREATEAPPOINTMENTDTO_H

#include "models/enums/AppointmentType.hpp"

#include <QDateTime>
#include <QJsonObject>
#include <QString>

class CreateAppointmentDTO
{
public:
    CreateAppointmentDTO(
        QDateTime appointmentDatetime,
        AppointmentType type,
        int patientId,
        QString observation);

    QJsonObject toJson() const;

    QDateTime appointmentDatetime;
    AppointmentType type;
    int patientId;
    QString observation;
};

#endif
