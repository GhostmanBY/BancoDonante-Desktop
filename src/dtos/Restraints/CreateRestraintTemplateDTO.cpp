#include "dtos/Restraints/CreateRestraintTemplateDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"

CreateRestraintTemplateDTO::CreateRestraintTemplateDTO(QString motivo, int defaultMonths)
    : motivo(std::move(motivo)),
      defaultMonths(defaultMonths)
{
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

QJsonObject CreateRestraintTemplateDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("motivo"), motivo);
    json.insert(QStringLiteral("default_months"), defaultMonths);
    return json;
}
