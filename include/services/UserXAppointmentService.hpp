#ifndef USERXAPPOINTMENTSERVICE_H
#define USERXAPPOINTMENTSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QObject>

class UserXAppointmentService : public QObject
{
    Q_OBJECT

public:
    explicit UserXAppointmentService(QObject *parent = nullptr);
    void create(int userId, int appointmentId);

signals:
    void createSucceeded();
    void failed(const ApiError &error);

private:
    ApiClient m_api;
};

#endif
