#include "dtos/Persones/UpdatePersonesDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdatePersonesDTO::UpdatePersonesDTO(
    int id,
    QString name,
    QString lastname,
    QString mail,
    QString phone,
    QString documentType,
    QString documentNumber,
    std::optional<QString> birthDate,
    std::optional<QString> birthPlace)
    : CreatePersonesDTO(
          std::move(name),
          std::move(lastname),
          std::move(mail),
          std::move(phone),
          std::move(documentType),
          std::move(documentNumber),
          std::move(birthDate),
          std::move(birthPlace)),
      id(id)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la persona"));
    }
}

QJsonObject UpdatePersonesDTO::toJson() const
{
    QJsonObject json = CreatePersonesDTO::toJson();
    json.insert(QStringLiteral("id"), id);
    return json;
}
