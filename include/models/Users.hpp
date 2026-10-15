#ifndef USERS_H
#define USERS_H

#include "models/Persones.hpp"
#include "models/enums/UserRole.hpp"

#include <optional>

class Users : public Persones
{
public:
    Users() = default;

    static Users fromJson(const QJsonObject &json);

    QJsonObject toJson() const override;
    QString toString() const override;

    QString username() const { return m_username; }
    QString hashPassword() const { return m_hashPassword; }
    std::optional<UserRole> role() const { return m_role; }

protected:
    QString m_username;
    QString m_hashPassword;
    std::optional<UserRole> m_role;
};

#endif
