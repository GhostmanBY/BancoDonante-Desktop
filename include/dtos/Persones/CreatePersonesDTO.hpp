#ifndef CREATEPERSONESDTO_H
#define CREATEPERSONESDTO_H

#include <QJsonObject>
#include <QString>
#include <optional>

class CreatePersonesDTO
{
public:
    CreatePersonesDTO(
        QString name,
        QString lastname,
        QString mail,
        QString phone,
        QString documentType,
        QString documentNumber,
        std::optional<QString> birthDate = std::nullopt,
        std::optional<QString> birthPlace = std::nullopt);

    virtual ~CreatePersonesDTO() = default;

    virtual QJsonObject toJson() const;

    QString name;
    QString lastname;
    QString mail;
    QString phone;
    QString documentType;
    QString documentNumber;
    std::optional<QString> birthDate;
    std::optional<QString> birthPlace;
};

#endif
