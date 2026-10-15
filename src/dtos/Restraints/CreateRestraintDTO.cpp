#include "dtos/Restraints/CreateRestraintDTO.hpp"

#include "exceptions/Validation/DateInThePastException.hpp"
#include "exceptions/Validation/InvalidDateOrderException.hpp"
#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"

CreateRestraintDTO::CreateRestraintDTO(
    QString motivo,
    QDateTime estimatedStartDate,
    QDateTime estimatedEndDate)
    : motivo(std::move(motivo)),
      estimatedStartDate(std::move(estimatedStartDate)),
      estimatedEndDate(std::move(estimatedEndDate))
{
    if (this->motivo.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("motivo"));
    }

    if (this->motivo.size() > 155) {
        throw InvalidValueException(QStringLiteral("motivo"));
    }

    if (this->estimatedStartDate < QDateTime::currentDateTime().date().startOfDay()) {
        throw DateInThePastException(QStringLiteral("estimated_start_date"));
    }

    if (this->estimatedEndDate < QDateTime::currentDateTime().date().startOfDay()) {
        throw DateInThePastException(QStringLiteral("estimated_end_date"));
    }

    if (this->estimatedStartDate > this->estimatedEndDate) {
        throw InvalidDateOrderException(
            QStringLiteral("estimated_start_date"),
            QStringLiteral("estimated_end_date"));
    }
}

QJsonObject CreateRestraintDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("motivo"), motivo);
    json.insert(QStringLiteral("estimated_start_date"),
                estimatedStartDate.toString(QStringLiteral("yyyy-MM-dd")));
    json.insert(QStringLiteral("estimated_end_date"),
                estimatedEndDate.toString(QStringLiteral("yyyy-MM-dd")));
    return json;
}
