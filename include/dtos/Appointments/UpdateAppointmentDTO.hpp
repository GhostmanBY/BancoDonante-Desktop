#ifndef UPDATEAPPOINTMENTDTO_H
#define UPDATEAPPOINTMENTDTO_H

#include "dtos/Appointments/CreateAppointmentDTO.hpp"
#include "models/enums/AppointmentState.hpp"

class UpdateAppointmentDTO : public CreateAppointmentDTO
{
public:
    UpdateAppointmentDTO(
        int id,
        QDateTime appointmentDatetime,
        AppointmentState state,
        AppointmentType type,
        int patientId,
        QString observation);

    QJsonObject toJson() const;

    int id;
    AppointmentState state;
};

#endif
