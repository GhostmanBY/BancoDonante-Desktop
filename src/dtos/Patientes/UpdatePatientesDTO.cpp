#include "dtos/Patientes/UpdatePatientesDTO.hpp"

#include "exceptions/Validation/OutOfRangeException.hpp"

UpdatePatientesDTO::UpdatePatientesDTO(
    int id,
    QString name,
    QString lastname,
    QString mail,
    QString phone,
    QString documentType,
    QString documentNumber,
    std::optional<QString> birthDate,
    std::optional<QString> birthPlace,
    PatientState stateDonation,
    Sex sex,
    double peso,
    double altura,
    BloodGroup bloodGroup)
    : UpdatePersonesDTO(
          id,
          std::move(name),
          std::move(lastname),
          std::move(mail),
          std::move(phone),
          std::move(documentType),
          std::move(documentNumber),
          std::move(birthDate),
          std::move(birthPlace)),
      stateDonation(stateDonation),
      sex(sex),
      peso(peso),
      altura(altura),
      bloodGroup(bloodGroup)
{
    if (this->peso < 30 || this->peso > 300) {
        throw OutOfRangeException(QStringLiteral("peso"), QStringLiteral("30 y 300 kg"));
    }

    if (this->altura < 100 || this->altura > 230) {
        throw OutOfRangeException(QStringLiteral("altura"), QStringLiteral("100 y 230 cm"));
    }
}

QJsonObject UpdatePatientesDTO::toJson() const
{
    QJsonObject json = UpdatePersonesDTO::toJson();
    json.insert(QStringLiteral("state_donation"), ::toString(stateDonation));
    json.insert(QStringLiteral("sex"), ::toString(sex));
    json.insert(QStringLiteral("peso"), peso);
    json.insert(QStringLiteral("altura"), altura);
    json.insert(QStringLiteral("blood_group"), ::toString(bloodGroup));
    return json;
}
