#include "models/Addresses.hpp"

#include "core/JsonUtils.hpp"

Addresses Addresses::fromJson(const QJsonObject &json)
{
    Addresses model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_personId = jsonutils::intOr(json, QStringLiteral("person_id"));
    model.m_street = jsonutils::stringOr(json, QStringLiteral("street"));
    model.m_streetNumber = jsonutils::optString(json, QStringLiteral("street_number"));
    model.m_neighborhood = jsonutils::optString(json, QStringLiteral("neighborhood"));
    model.m_city = jsonutils::optString(json, QStringLiteral("city"));
    model.m_postalCode = jsonutils::optString(json, QStringLiteral("postal_code"));
    model.m_country = jsonutils::optString(json, QStringLiteral("country"));
    model.m_floor = jsonutils::optString(json, QStringLiteral("floor"));
    model.m_apartment = jsonutils::optString(json, QStringLiteral("apartment"));
    model.m_betweenStreets = jsonutils::optString(json, QStringLiteral("between_streets"));
    model.m_isCurrent = jsonutils::boolOr(json, QStringLiteral("is_current"), true);
    return model;
}

QJsonObject Addresses::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("person_id"), m_personId);
    json.insert(QStringLiteral("street"), m_street);
    json.insert(QStringLiteral("street_number"),
                m_streetNumber.has_value() ? *m_streetNumber : QJsonValue());
    json.insert(QStringLiteral("neighborhood"),
                m_neighborhood.has_value() ? *m_neighborhood : QJsonValue());
    json.insert(QStringLiteral("city"), m_city.has_value() ? *m_city : QJsonValue());
    json.insert(QStringLiteral("postal_code"),
                m_postalCode.has_value() ? *m_postalCode : QJsonValue());
    json.insert(QStringLiteral("country"), m_country.has_value() ? *m_country : QJsonValue());
    json.insert(QStringLiteral("floor"), m_floor.has_value() ? *m_floor : QJsonValue());
    json.insert(QStringLiteral("apartment"),
                m_apartment.has_value() ? *m_apartment : QJsonValue());
    json.insert(QStringLiteral("between_streets"),
                m_betweenStreets.has_value() ? *m_betweenStreets : QJsonValue());
    json.insert(QStringLiteral("is_current"), m_isCurrent);
    return json;
}

QString Addresses::toString() const
{
    return QStringLiteral("Addresses(id=%1, person_id=%2, street=%3 %4, city=%5, is_current=%6)")
        .arg(m_id)
        .arg(m_personId)
        .arg(m_street, m_streetNumber.value_or(QString()))
        .arg(m_city.value_or(QString()))
        .arg(m_isCurrent ? QStringLiteral("true") : QStringLiteral("false"));
}
