#ifndef USERXAPPOINTMENT_H
#define USERXAPPOINTMENT_H

#include <QJsonObject>

class UserXAppointment
{
public:
    UserXAppointment() = default;

    static UserXAppointment fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    int userId() const { return m_userId; }
    int appointmentId() const { return m_appointmentId; }

private:
    int m_id = 0;
    int m_userId = 0;
    int m_appointmentId = 0;
};

#endif
