#include "models/Users.hpp"

#include "core/JsonUtils.hpp"

Users Users::fromJson(const QJsonObject &json)
{
    Users model;
    static_cast<Persones &>(model) = Persones::fromJson(json);
    model.m_username = jsonutils::stringOr(json, QStringLiteral("username"));
    model.m_hashPassword = jsonutils::stringOr(json, QStringLiteral("hash_password"));

    const auto roleValue = jsonutils::optString(json, QStringLiteral("role"));
    if (roleValue.has_value() && !roleValue->isEmpty()) {
        model.m_role = userRoleFromString(*roleValue, UserRole::ADMIN);
    }
    return model;
}

QJsonObject Users::toJson() const
{
    QJsonObject json = Persones::toJson();
    json.insert(QStringLiteral("username"), m_username);
    json.insert(QStringLiteral("role"), m_role.has_value() ? ::toString(*m_role) : QJsonValue());
    return json;
}

QString Users::toString() const
{
    return QStringLiteral("Users(id=%1, name=%2, lastname=%3, mail=%4, username=%5, role=%6)")
        .arg(m_id)
        .arg(m_name, m_lastname, m_mail, m_username)
        .arg(m_role.has_value() ? ::toString(*m_role) : QStringLiteral("null"));
}
