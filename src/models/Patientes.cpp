#include "models/Patientes.hpp"

#include "core/JsonUtils.hpp"

Patientes Patientes::fromJson(const QJsonObject &json)
{
    Patientes model;
    static_cast<Persones &>(model) = Persones::fromJson(json);
    model.m_stateDonation = patientStateFromString(
        jsonutils::stringOr(json, QStringLiteral("state_donation")),
        PatientState::NO_APTO);
    model.m_sex = sexFromString(
        jsonutils::stringOr(json, QStringLiteral("sex")),
        Sex::M);
    model.m_peso = jsonutils::optDouble(json, QStringLiteral("peso"));
    model.m_altura = jsonutils::optDouble(json, QStringLiteral("altura"));

    const auto bloodGroupRaw = jsonutils::optString(json, QStringLiteral("blood_group"));
    model.m_bloodGroup = bloodGroupRaw.has_value()
        ? std::optional(bloodGroupFromString(*bloodGroupRaw, BloodGroup::O_POS))
        : std::nullopt;

    const auto lastDonationRaw = jsonutils::optString(json, QStringLiteral("last_donation_date"));
    model.m_lastDonationDate = lastDonationRaw.has_value()
        ? std::optional(jsonutils::parseDateTime(*lastDonationRaw))
        : std::nullopt;

    return model;
}

QJsonObject Patientes::toJson() const
{
    QJsonObject json = Persones::toJson();
    json.insert(QStringLiteral("state_donation"), ::toString(m_stateDonation));
    json.insert(QStringLiteral("sex"), ::toString(m_sex));
    json.insert(QStringLiteral("peso"), m_peso.has_value() ? *m_peso : QJsonValue());
    json.insert(QStringLiteral("altura"), m_altura.has_value() ? *m_altura : QJsonValue());
    json.insert(
        QStringLiteral("blood_group"),
        m_bloodGroup.has_value() ? ::toString(*m_bloodGroup) : QJsonValue()
    );
    return json;
}

QString Patientes::toString() const
{
    return QStringLiteral("Patientes(id=%1, name=%2, lastname=%3, mail=%4, phone=%5, document=%6 %7, state_donation=%8, sex=%9)")
        .arg(m_id)
        .arg(m_name, m_lastname, m_mail, m_phone, m_documentType, m_documentNumber)
        .arg(::toString(m_stateDonation), ::toString(m_sex));
}
