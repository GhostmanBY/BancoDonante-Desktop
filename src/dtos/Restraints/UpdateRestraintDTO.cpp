#include "dtos/Restraints/UpdateRestraintDTO.hpp"

#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateRestraintDTO::UpdateRestraintDTO(
    int id,
    QString motivo,
    QDateTime estimatedStartDate,
    QDateTime estimatedEndDate)
    : CreateRestraintDTO(
          std::move(motivo),
          std::move(estimatedStartDate),
          std::move(estimatedEndDate)),
      id(id)
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id de la restricción"));
    }
}

QJsonObject UpdateRestraintDTO::toJson() const
{
    QJsonObject json = CreateRestraintDTO::toJson();
    json.insert(QStringLiteral("id"), id);
    return json;
}
