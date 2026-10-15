#include "models/Persones.hpp"

#include "core/JsonUtils.hpp"

Persones Persones::fromJson(const QJsonObject &json)
{
    Persones model;
    model.m_id = jsonutils::intOr(json, QStringLiteral("id"));
    model.m_name = jsonutils::stringOr(json, QStringLiteral("name"));
    model.m_lastname = jsonutils::stringOr(json, QStringLiteral("lastname"));
    model.m_mail = jsonutils::stringOr(json, QStringLiteral("mail"));
    model.m_phone = jsonutils::stringOr(json, QStringLiteral("phone"));
    model.m_documentType = jsonutils::stringOr(json, QStringLiteral("document_type"));
    model.m_documentNumber = jsonutils::stringOr(json, QStringLiteral("document_number"));

    const auto birthDate = jsonutils::optString(json, QStringLiteral("birth_date"));
    if (birthDate.has_value()) {
        model.m_birthDate = jsonutils::parseDateTime(*birthDate);
    }

    model.m_birthPlace = jsonutils::optString(json, QStringLiteral("birth_place"));
    model.m_isActive = jsonutils::boolOr(json, QStringLiteral("isActive"));
    return model;
}

QJsonObject Persones::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), m_id);
    json.insert(QStringLiteral("name"), m_name);
    json.insert(QStringLiteral("lastname"), m_lastname);
    json.insert(QStringLiteral("mail"), m_mail);
    json.insert(QStringLiteral("phone"), m_phone);
    json.insert(QStringLiteral("document_type"), m_documentType);
    json.insert(QStringLiteral("document_number"), m_documentNumber);
    json.insert(QStringLiteral("birth_date"),
                m_birthDate.has_value() ? m_birthDate->toString(Qt::ISODate) : QJsonValue());
    json.insert(QStringLiteral("birth_place"),
                m_birthPlace.has_value() ? *m_birthPlace : QJsonValue());
    json.insert(QStringLiteral("isActive"), m_isActive);
    return json;
}

QString Persones::toString() const
{
    return QStringLiteral("Persones(id=%1, name=%2, lastname=%3, mail=%4, phone=%5, document=%6 %7, isActive=%8)")
        .arg(m_id)
        .arg(m_name, m_lastname, m_mail, m_phone, m_documentType, m_documentNumber)
        .arg(m_isActive ? QStringLiteral("true") : QStringLiteral("false"));
}
