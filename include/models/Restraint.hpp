#ifndef RESTRAINT_H
#define RESTRAINT_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>

class Restraint
{
public:
    Restraint() = default;

    static Restraint fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    QString motivo() const { return m_motivo; }
    QDateTime estimatedStartDate() const { return m_estimatedStartDate; }
    QDateTime estimatedEndDate() const { return m_estimatedEndDate; }
    bool isActive() const { return m_isActive; }

private:
    int m_id = 0;
    QString m_motivo;
    QDateTime m_estimatedStartDate;
    QDateTime m_estimatedEndDate;
    bool m_isActive = true;
};

#endif
