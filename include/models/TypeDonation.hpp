#ifndef TYPEDONATION_H
#define TYPEDONATION_H

#include <QJsonObject>
#include <QString>

class TypeDonation
{
public:
    TypeDonation() = default;

    static TypeDonation fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    QString name() const { return m_name; }
    bool isActive() const { return m_isActive; }

private:
    int m_id = 0;
    QString m_name;
    bool m_isActive = true;
};

#endif
