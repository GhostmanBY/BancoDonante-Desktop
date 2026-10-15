#include "dtos/Persones/ActivePersoneDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

ActivePersoneDTO::ActivePersoneDTO(int id, bool isActive)
    : id(id), isActive(isActive)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la persona"));
    }
}

QJsonObject ActivePersoneDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), id);
    json.insert(QStringLiteral("isActive"), isActive);
    return json;
}
