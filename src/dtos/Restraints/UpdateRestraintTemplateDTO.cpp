#include "dtos/Restraints/UpdateRestraintTemplateDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"

UpdateRestraintTemplateDTO::UpdateRestraintTemplateDTO(int id, QString motivo, int defaultMonths)
    : id(id),
      motivo(std::move(motivo)),
      defaultMonths(defaultMonths)
{
    if (this->id < 0) {
        throw InvalidValueException(QStringLiteral("id de la plantilla de restricción"));
    }

    if (this->motivo.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("motivo"));
    }

    if (this->motivo.size() > 155) {
        throw InvalidValueException(QStringLiteral("motivo"));
    }

    if (this->defaultMonths < 1 || this->defaultMonths > 120) {
        throw InvalidValueException(QStringLiteral("default_months"));
    }
}

QJsonObject UpdateRestraintTemplateDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("motivo"), motivo);
    json.insert(QStringLiteral("default_months"), defaultMonths);
    return json;
}
