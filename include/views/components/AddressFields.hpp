#ifndef ADDRESSFIELDS_H
#define ADDRESSFIELDS_H

#include "dtos/Addresses/CreateAddressDTO.hpp"

#include <QGroupBox>
#include <optional>

class QCheckBox;
class QLineEdit;

class AddressFields : public QGroupBox
{
    Q_OBJECT

public:
    explicit AddressFields(QWidget *parent = nullptr);

    void setChecked(bool checked);
    bool isChecked() const;
    bool validate();
    std::optional<CreateAddressDTO> toDto(int personId) const;

    QString street() const;

private:
    QCheckBox *m_enabled = nullptr;
    QLineEdit *m_street = nullptr;
    QLineEdit *m_streetNumber = nullptr;
    QLineEdit *m_neighborhood = nullptr;
    QLineEdit *m_city = nullptr;
    QLineEdit *m_postalCode = nullptr;
    QLineEdit *m_floor = nullptr;
    QLineEdit *m_apartment = nullptr;
    QLineEdit *m_betweenStreets = nullptr;
};

#endif
