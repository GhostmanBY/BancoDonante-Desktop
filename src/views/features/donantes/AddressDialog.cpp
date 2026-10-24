#include "views/features/donantes/AddressDialog.hpp"

#include "services/DonorService.hpp"
#include "views/components/AddressFields.hpp"
#include "views/components/Toast.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

AddressDialog::AddressDialog(int personId, QWidget *parent)
    : QDialog(parent),
      m_service(new DonorService(this)),
      m_personId(personId)
{
    setWindowTitle(QStringLiteral("Agregar dirección"));
    setModal(true);
    setMinimumWidth(560);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("Nueva dirección del donante"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    m_fields = new AddressFields(this);
    m_fields->setTitle(QStringLiteral("Datos de la dirección"));
    m_fields->setChecked(true);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(QStringLiteral("Guardar dirección"), QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(m_fields);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &AddressDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &AddressDialog::reject);

    connect(m_service, &DonorService::addressCreated, this, [this]() {
        Toast::show(this, QStringLiteral("Dirección guardada"), Toast::Type::Success);
        emit saved();
        accept();
    });
    connect(m_service, &DonorService::failed, this, &AddressDialog::onFailed);
}

void AddressDialog::submit()
{
    if (!m_fields->validate()) {
        Toast::show(this, QStringLiteral("La calle es obligatoria"), Toast::Type::Error);
        return;
    }

    const auto dto = m_fields->toDto(m_personId);
    if (!dto.has_value()) {
        return;
    }

    m_acceptBtn->setEnabled(false);
    m_service->createAddress(*dto);
}

void AddressDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo guardar la dirección")
        : error.detail;
    Toast::show(this, message, Toast::Type::Error);
}
