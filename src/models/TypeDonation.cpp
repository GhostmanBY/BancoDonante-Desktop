#include "models/TypeDonation.hpp"

#include "core/JsonUtils.hpp"

TypeDonation TypeDonation::fromJson(const QJsonObject &json)
{
    TypeDonation model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_name = jsonutils::stringOr(json, QStringLiteral("name"));
    model.m_isActive = jsonutils::boolOr(json, QStringLiteral("isActive"), true);
    return model;
}

QJsonObject TypeDonation::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("name"), m_name);
    json.insert(QStringLiteral("isActive"), m_isActive);
    return json;
}

QString TypeDonation::toString() const
{
    return QStringLiteral("TypeDonation(id=%1, name=%2, isActive=%3)")
        .arg(m_id)
        .arg(m_name)
        .arg(m_isActive ? QStringLiteral("true") : QStringLiteral("false"));
}
