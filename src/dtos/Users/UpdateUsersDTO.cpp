#include "dtos/Users/UpdateUsersDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateUsersDTO::UpdateUsersDTO(
    int id,
    QString name,
    QString lastname,
    QString mail,
    QString phone,
    QString documentType,
    QString documentNumber,
    std::optional<QString> birthDate,
    std::optional<QString> birthPlace,
    QString username,
    QString hashPassword,
    UserRole role)
    : UpdatePersonesDTO(
          id,
          std::move(name),
          std::move(lastname),
          std::move(mail),
          std::move(phone),
          std::move(documentType),
          std::move(documentNumber),
          std::move(birthDate),
          std::move(birthPlace)),
      username(std::move(username)),
      hashPassword(std::move(hashPassword)),
      role(role)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id del usuario"));
    }

    if (this->username.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("username"));
    }
    if (this->username.size() > 155) {
        throw InvalidValueException(QStringLiteral("username"));
    }

    if (this->hashPassword.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("hash_password"));
    }
    if (this->hashPassword.size() > 255) {
        throw InvalidValueException(QStringLiteral("hash_password"));
    }
}

QJsonObject UpdateUsersDTO::toJson() const
{
    QJsonObject json = UpdatePersonesDTO::toJson();
    json.insert(QStringLiteral("username"), username);
    json.insert(QStringLiteral("hash_password"), hashPassword);
    json.insert(QStringLiteral("role"), ::toString(role));
    return json;
}
