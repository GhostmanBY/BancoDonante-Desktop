#include "dtos/Persones/CreatePersonesDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"

CreatePersonesDTO::CreatePersonesDTO(
    QString name,
    QString lastname,
    QString mail,
    QString phone,
    QString documentType,
    QString documentNumber,
    std::optional<QString> birthDate,
    std::optional<QString> birthPlace)
    : name(std::move(name)),
      lastname(std::move(lastname)),
      mail(std::move(mail)),
      phone(std::move(phone)),
      documentType(std::move(documentType)),
      documentNumber(std::move(documentNumber)),
      birthDate(std::move(birthDate)),
      birthPlace(std::move(birthPlace))
{
    if (this->name.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("name"));
    }
    if (this->name.size() > 155) {
        throw InvalidValueException(QStringLiteral("name"));
    }

    if (this->lastname.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("lastname"));
    }
    if (this->lastname.size() > 155) {
        throw InvalidValueException(QStringLiteral("lastname"));
    }

    if (this->mail.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("mail"));
    }
    if (this->mail.size() > 255) {
        throw InvalidValueException(QStringLiteral("mail"));
    }

    if (this->phone.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("phone"));
    }
    if (this->phone.size() > 155) {
        throw InvalidValueException(QStringLiteral("phone"));
    }

    if (this->documentType.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("document_type"));
    }
    if (this->documentType.size() > 20) {
        throw InvalidValueException(QStringLiteral("document_type"));
    }

    if (this->documentNumber.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("document_number"));
    }
    if (this->documentNumber.size() > 30) {
        throw InvalidValueException(QStringLiteral("document_number"));
    }

    if (this->birthDate.has_value() && this->birthDate->trimmed().isEmpty()) {
        this->birthDate = std::nullopt;
    }

    if (this->birthPlace.has_value() && this->birthPlace->size() > 150) {
        throw InvalidValueException(QStringLiteral("birth_place"));
    }
}

QJsonObject CreatePersonesDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("name"), name);
    json.insert(QStringLiteral("lastname"), lastname);
    json.insert(QStringLiteral("mail"), mail);
    json.insert(QStringLiteral("phone"), phone);
    json.insert(QStringLiteral("document_type"), documentType);
    json.insert(QStringLiteral("document_number"), documentNumber);
    json.insert(QStringLiteral("birth_date"), birthDate.has_value() ? *birthDate : QJsonValue());
    json.insert(QStringLiteral("birth_place"), birthPlace.has_value() ? *birthPlace : QJsonValue());
    return json;
}
