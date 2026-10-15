#ifndef RESTRAINTXAPPOINTMENT_H
#define RESTRAINTXAPPOINTMENT_H

#include <QJsonObject>

class RestraintXAppointment
{
public:
    RestraintXAppointment() = default;

    static RestraintXAppointment fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    int restraintId() const { return m_restraintId; }
    int appointmentId() const { return m_appointmentId; }

private:
    int m_id = 0;
    int m_restraintId = 0;
    int m_appointmentId = 0;
};

#endif
