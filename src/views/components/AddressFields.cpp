#include "views/components/AddressFields.hpp"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStyle>
#include <QVBoxLayout>

namespace {

std::optional<QString> textOrNull(QLineEdit *field)
{
    const QString text = field->text().trimmed();
    return text.isEmpty() ? std::nullopt : std::optional(text);
}

} // namespace

AddressFields::AddressFields(QWidget *parent)
    : QGroupBox(parent)
{
    setTitle(QStringLiteral("Dirección (opcional)"));

    m_enabled = new QCheckBox(QStringLiteral("Agregar dirección junto con el donante"), this);

    m_street = new QLineEdit(this);
    m_street->setPlaceholderText(QStringLiteral("San Juan"));
    m_streetNumber = new QLineEdit(this);
    m_streetNumber->setPlaceholderText(QStringLiteral("Número"));
    m_neighborhood = new QLineEdit(this);
    m_neighborhood->setPlaceholderText(QStringLiteral("Barrio"));
    m_city = new QLineEdit(this);
    m_city->setPlaceholderText(QStringLiteral("Ciudad"));
    m_postalCode = new QLineEdit(this);
    m_postalCode->setPlaceholderText(QStringLiteral("7600"));
    m_floor = new QLineEdit(this);
    m_floor->setPlaceholderText(QStringLiteral("4"));
    m_apartment = new QLineEdit(this);
    m_apartment->setPlaceholderText(QStringLiteral("B"));
    m_betweenStreets = new QLineEdit(this);
    m_betweenStreets->setPlaceholderText(QStringLiteral("Colón & Belgrano"));

    auto *fields = new QWidget(this);
    auto *grid = new QGridLayout(fields);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(10);

    auto addRow = [fields, grid](int row, int col, const QString &label, QLineEdit *field) {
        auto *box = new QVBoxLayout();
        box->setSpacing(4);
        auto *caption = new QLabel(label, fields);
        caption->setProperty("role", "hint");
        box->addWidget(caption);
        box->addWidget(field);
        grid->addLayout(box, row, col);
    };

    addRow(0, 0, QStringLiteral("Calle *"), m_street);
    addRow(0, 1, QStringLiteral("Número"), m_streetNumber);
    addRow(0, 2, QStringLiteral("Barrio"), m_neighborhood);
    addRow(1, 0, QStringLiteral("Ciudad"), m_city);
    addRow(1, 1, QStringLiteral("Código postal"), m_postalCode);
    addRow(1, 2, QStringLiteral("Piso"), m_floor);
    addRow(2, 0, QStringLiteral("Departamento"), m_apartment);
    addRow(2, 1, QStringLiteral("Entre calles"), m_betweenStreets);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);
    layout->addWidget(m_enabled);
    layout->addWidget(fields);

    connect(m_enabled, &QCheckBox::toggled, fields, &QWidget::setEnabled);
    fields->setEnabled(false);
}

void AddressFields::setChecked(bool checked)
{
    m_enabled->setChecked(checked);
}

bool AddressFields::isChecked() const
{
    return m_enabled->isChecked();
}

QString AddressFields::street() const
{
    return m_street->text().trimmed();
}

bool AddressFields::validate()
{
    if (!m_enabled->isChecked()) {
        return true;
    }

    const bool valid = !m_street->text().trimmed().isEmpty();
    m_street->setProperty("error", valid ? "false" : "true");
    m_street->style()->unpolish(m_street);
    m_street->style()->polish(m_street);
    return valid;
}

std::optional<CreateAddressDTO> AddressFields::toDto(int personId) const
{
    if (!m_enabled->isChecked() || m_street->text().trimmed().isEmpty()) {
        return std::nullopt;
    }

    return CreateAddressDTO(
        personId,
        m_street->text().trimmed(),
        textOrNull(m_streetNumber),
        textOrNull(m_neighborhood),
        textOrNull(m_city),
        textOrNull(m_postalCode),
        textOrNull(m_floor),
        textOrNull(m_apartment),
        textOrNull(m_betweenStreets),
        true);
}
