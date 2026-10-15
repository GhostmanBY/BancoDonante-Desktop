#include "dtos/Donations/UpdateTypeDonationDTO.hpp"

#include "exceptions/Validation/InvalidValueException.hpp"
#include "exceptions/Validation/MissingValueException.hpp"
#include "exceptions/Validation/NegativeNumberException.hpp"

UpdateTypeDonationDTO::UpdateTypeDonationDTO(int id, QString name)
    : id(id), name(std::move(name))
{
    if (this->id < 0) {
        throw NegativeNumberException(QStringLiteral("id del tipo de donación"));
    }

    if (this->name.trimmed().isEmpty()) {
        throw MissingValueException(QStringLiteral("name"));
    }

    if (this->name.size() > 120) {
        throw InvalidValueException(QStringLiteral("name"));
    }
}

QJsonObject UpdateTypeDonationDTO::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("id"), id);
    json.insert(QStringLiteral("name"), name);
    return json;
}
