#ifndef ADDRESSES_H
#define ADDRESSES_H

#include <QJsonObject>
#include <QString>
#include <optional>

class Addresses
{
public:
    Addresses() = default;

    static Addresses fromJson(const QJsonObject &json);

    QJsonObject toJson() const;
    QString toString() const;

    int id() const { return m_id; }
    int personId() const { return m_personId; }
    QString street() const { return m_street; }
    std::optional<QString> streetNumber() const { return m_streetNumber; }
    std::optional<QString> neighborhood() const { return m_neighborhood; }
    std::optional<QString> city() const { return m_city; }
    std::optional<QString> postalCode() const { return m_postalCode; }
    std::optional<QString> country() const { return m_country; }
    std::optional<QString> floor() const { return m_floor; }
    std::optional<QString> apartment() const { return m_apartment; }
    std::optional<QString> betweenStreets() const { return m_betweenStreets; }
    bool isCurrent() const { return m_isCurrent; }

private:
    int m_id = 0;
    int m_personId = 0;
    QString m_street;
    std::optional<QString> m_streetNumber;
    std::optional<QString> m_neighborhood;
    std::optional<QString> m_city;
    std::optional<QString> m_postalCode;
    std::optional<QString> m_country;
    std::optional<QString> m_floor;
    std::optional<QString> m_apartment;
    std::optional<QString> m_betweenStreets;
    bool m_isCurrent = true;
};

#endif
