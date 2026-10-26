#include "views/features/turnos/RescheduleTurnoDialog.hpp"

#include "dtos/Appointments/UpdateAppointmentDTO.hpp"
#include "views/components/Toast.hpp"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>

RescheduleTurnoDialog::RescheduleTurnoDialog(const TurnoItem &item, QWidget *parent)
    : QDialog(parent),
      m_item(item),
      m_service(new AppointmentService(this))
{
    setWindowTitle(QStringLiteral("Reprogramar turno"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    connect(m_service, &AppointmentService::updateSucceeded, this, [this]() {
        Toast::show(
            this,
            QStringLiteral("Turno reprogramado para %1 %2")
                .arg(
                    m_date->date().toString(QStringLiteral("dd/MM/yyyy")),
                    m_time->currentText()),
            Toast::Type::Success);
        emit rescheduled();
        accept();
    });
    connect(m_service, &AppointmentService::failed, this, &RescheduleTurnoDialog::onFailed);
    connect(m_service, &AppointmentService::availabilityChecked, this, [this](bool available) {
        if (!available) {
            m_date->setEnabled(true);
            m_time->setEnabled(true);
            m_acceptBtn->setEnabled(true);
            const QString message =
                QStringLiteral("Ya existe un turno en esa fecha y horario.");
            m_formError->setText(message);
            m_formError->setVisible(true);
            Toast::show(this, message, Toast::Type::Error);
            return;
        }

        m_service->update(UpdateAppointmentDTO(
            m_item.id,
            m_pendingDatetime,
            AppointmentState::RESCHEDULED,
            m_item.type,
            m_item.patientId,
            m_item.observation));
    });
}

void RescheduleTurnoDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        QStringLiteral("Reprogramar turno de %1 %2")
            .arg(m_item.patientName, m_item.patientLastname),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(
        QStringLiteral("Actual: %1 · %2")
            .arg(
                m_item.datetime.isValid()
                    ? m_item.datetime.toLocalTime().toString(
                          QStringLiteral("dd/MM/yyyy HH:mm"))
                    : QStringLiteral("—"),
                m_item.type == AppointmentType::DONATION
                    ? QStringLiteral("Turno de donación")
                    : QStringLiteral("Control médico")),
        this);
    subtitle->setProperty("role", "hint");

    auto *form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    auto caption = [](const QString &text) {
        auto *label = new QLabel(text);
        label->setProperty("role", "hint");
        return label;
    };

    auto *dateRow = new QWidget(this);
    auto *dateLayout = new QHBoxLayout(dateRow);
    dateLayout->setContentsMargins(0, 0, 0, 0);
    dateLayout->setSpacing(8);

    m_date = new QDateEdit(dateRow);
    m_date->setObjectName(QStringLiteral("appointmentDate"));
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat(QStringLiteral("dddd d 'de' MMMM 'de' yyyy"));
    m_date->setMinimumWidth(280);

    QDate newDate = m_item.datetime.isValid()
        ? m_item.datetime.toLocalTime().date()
        : QDate::currentDate();
    if (newDate < QDate::currentDate()) {
        newDate = QDate::currentDate().addDays(1);
    }
    m_date->setMinimumDate(QDate::currentDate());
    m_date->setDate(newDate);

    m_time = new QComboBox(dateRow);
    m_time->setMinimumWidth(110);
    for (int hour = 8; hour <= 19; ++hour) {
        m_time->addItem(QStringLiteral("%1:00").arg(hour, 2, 10, QLatin1Char('0')));
        if (hour < 19) {
            m_time->addItem(QStringLiteral("%1:30").arg(hour, 2, 10, QLatin1Char('0')));
        }
    }
    const QString currentTime =
        m_item.datetime.toLocalTime().toString(QStringLiteral("HH:mm"));
    const int existing = m_time->findText(currentTime);
    m_time->setCurrentIndex(existing >= 0
        ? existing
        : m_time->findText(QStringLiteral("09:00")));

    dateLayout->addWidget(m_date, 1);
    dateLayout->addWidget(m_time);

    form->addRow(caption(QStringLiteral("Nueva fecha *")), m_date);
    form->addRow(caption(QStringLiteral("Nueva hora *")), m_time);

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(QStringLiteral("Reprogramar"), QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addLayout(form);
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &RescheduleTurnoDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &RescheduleTurnoDialog::reject);
}

void RescheduleTurnoDialog::submit()
{
    const QStringList timeParts = m_time->currentText().split(QLatin1Char(':'));
    const QTime time(timeParts.value(0).toInt(), timeParts.value(1).toInt());
    const QDateTime when(m_date->date(), time);

    if (when < QDateTime::currentDateTime()) {
        m_formError->setText(QStringLiteral("La fecha y hora del turno no puede ser en el pasado."));
        m_formError->setVisible(true);
        return;
    }
    m_formError->setVisible(false);
    m_pendingDatetime = when;
    m_date->setEnabled(false);
    m_time->setEnabled(false);
    m_acceptBtn->setEnabled(false);
    m_service->checkAvailability(when, m_item.id);
}

void RescheduleTurnoDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    m_date->setEnabled(true);
    m_time->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo reprogramar el turno")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
