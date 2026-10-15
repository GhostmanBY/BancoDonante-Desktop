#include "dtos/Patientes/DeactivePatientDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

DeactivePatientDTO::DeactivePatientDTO(int id, bool isActive)
    : id(id), isActive(isActive)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id del paciente"));
    }
}

QJsonObject DeactivePatientDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), id);
    json.insert(QStringLiteral("isActive"), isActive);
    return json;
}
