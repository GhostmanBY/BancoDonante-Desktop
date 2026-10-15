#ifndef PATIENTES_H
#define PATIENTES_H

#include "models/Persones.hpp"
#include "models/enums/PatientState.hpp"
#include "models/enums/Sex.hpp"

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

protected:
    PatientState m_stateDonation = PatientState::NO_APTO;
    Sex m_sex = Sex::M;
    std::optional<double> m_peso;
    std::optional<double> m_altura;
};

#endif
