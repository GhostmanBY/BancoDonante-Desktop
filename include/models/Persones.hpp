#ifndef PERSONES_H
#define PERSONES_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <optional>

class Persones
{
public:
    Persones() = default;
    virtual ~Persones() = default;

    static Persones fromJson(const QJsonObject &json);

    virtual QJsonObject toJson() const;
    virtual QString toString() const;

    int id() const { return m_id; }
    QString name() const { return m_name; }
    QString lastname() const { return m_lastname; }
    QString mail() const { return m_mail; }
    QString phone() const { return m_phone; }
    QString documentType() const { return m_documentType; }
    QString documentNumber() const { return m_documentNumber; }
    std::optional<QDateTime> birthDate() const { return m_birthDate; }
    std::optional<QString> birthPlace() const { return m_birthPlace; }
    bool isActive() const { return m_isActive; }

protected:
    int m_id = 0;
    QString m_name;
    QString m_lastname;
    QString m_mail;
    QString m_phone;
    QString m_documentType;
    QString m_documentNumber;
    std::optional<QDateTime> m_birthDate;
    std::optional<QString> m_birthPlace;
    bool m_isActive = false;
};

#endif
