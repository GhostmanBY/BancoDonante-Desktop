#include "views/features/turnos/CompleteTurnoDialog.hpp"

#include "dtos/Appointments/UpdateAppointmentDTO.hpp"
#include "views/components/Toast.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLocale>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

CompleteTurnoDialog::CompleteTurnoDialog(const TurnoItem &item, QWidget *parent)
    : QDialog(parent),
      m_item(item),
      m_service(new AppointmentService(this))
{
    setWindowTitle(QStringLiteral("Completar turno"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    connect(m_service, &AppointmentService::updateSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Turno completado"), Toast::Type::Success);
        emit completed();
        accept();
    });
    connect(m_service, &AppointmentService::failed, this, &CompleteTurnoDialog::onFailed);
}

void CompleteTurnoDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        QStringLiteral("Completar turno de %1 %2")
            .arg(m_item.patientName, m_item.patientLastname),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(
        QStringLiteral("%1 · %2")
            .arg(
                m_item.datetime.isValid()
                    ? QLocale().toString(
                          m_item.datetime.toLocalTime(),
                          QStringLiteral("dddd d 'de' MMMM 'de' yyyy, HH:mm"))
                    : QStringLiteral("—"),
                m_item.type == AppointmentType::DONATION
                    ? QStringLiteral("Turno de donación")
                    : QStringLiteral("Control médico")),
        this);
    subtitle->setProperty("role", "hint");

    auto *observationLabel = new QLabel(QStringLiteral("Observación"), this);
    observationLabel->setProperty("role", "hint");

    m_observation = new QLineEdit(this);
    m_observation->setMaxLength(255);
    m_observation->setClearButtonEnabled(true);
    m_observation->setPlaceholderText(
        QStringLiteral("Motivo del cierre, novedades de la extracción… (opcional)"));
    m_observation->setText(m_item.observation);

    auto *counter = new QLabel(QStringLiteral("Máximo 255 caracteres."), this);
    counter->setProperty("role", "hint");

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(QStringLiteral("Completar turno"), QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(observationLabel);
    layout->addWidget(m_observation);
    layout->addWidget(counter);
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &CompleteTurnoDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &CompleteTurnoDialog::reject);
}

void CompleteTurnoDialog::submit()
{
    m_formError->setVisible(false);
    m_acceptBtn->setEnabled(false);

    m_service->update(UpdateAppointmentDTO(
        m_item.id,
        m_item.datetime,
        AppointmentState::COMPLETED,
        m_item.type,
        m_item.patientId,
        m_observation->text().trimmed()));
}

void CompleteTurnoDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo completar el turno")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
