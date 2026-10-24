#ifndef PATIENTES_H
#define PATIENTES_H

#include "models/Persones.hpp"
#include "models/enums/BloodGroup.hpp"
#include "models/enums/PatientState.hpp"
#include "models/enums/Sex.hpp"

#include <QDateTime>
#include <optional>

class Patientes : public Persones
{
public:
    Patientes() = default;

    static Patientes fromJson(const QJsonObject &json);

    QJsonObject toJson() const override;
    QString toString() const override;

    PatientState stateDonation() const { return m_stateDonation; }
    Sex sex() const { return m_sex; }
    std::optional<double> peso() const { return m_peso; }
    std::optional<double> altura() const { return m_altura; }
    std::optional<BloodGroup> bloodGroup() const { return m_bloodGroup; }
    std::optional<QDateTime> lastDonationDate() const { return m_lastDonationDate; }

    void setStateDonation(PatientState state) { m_stateDonation = state; }
    void setBloodGroup(BloodGroup group) { m_bloodGroup = group; }
    void setPeso(double peso) { m_peso = peso; }
    void setAltura(double altura) { m_altura = altura; }
    void setSex(Sex sex) { m_sex = sex; }
    void setLastDonationDate(const std::optional<QDateTime> &date) { m_lastDonationDate = date; }

protected:
    PatientState m_stateDonation = PatientState::NO_APTO;
    Sex m_sex = Sex::M;
    std::optional<double> m_peso;
    std::optional<double> m_altura;
    std::optional<BloodGroup> m_bloodGroup;
    std::optional<QDateTime> m_lastDonationDate;
};

#endif
