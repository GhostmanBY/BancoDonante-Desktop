#ifndef DONATIONS_H
#define DONATIONS_H

#include <QJsonObject>

class Donations
{
public:
    Donations() = default;

    static Donations fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    int appointmentId() const { return m_appointmentId; }
    int typeDonationId() const { return m_typeDonationId; }
    int cycleId() const { return m_cycleId; }

private:
    int m_id = 0;
    int m_appointmentId = 0;
    int m_typeDonationId = 0;
    int m_cycleId = 0;
};

#endif
