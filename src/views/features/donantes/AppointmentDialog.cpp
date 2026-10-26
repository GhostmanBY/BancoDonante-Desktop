#include "views/features/donantes/AppointmentDialog.hpp"

#include "core/JsonUtils.hpp"
#include "dtos/Appointments/CreateAppointmentDTO.hpp"
#include "models/enums/AppointmentType.hpp"
#include "services/AppointmentService.hpp"
#include "services/DonorService.hpp"
#include "views/components/Toast.hpp"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTime>
#include <QVBoxLayout>

AppointmentDialog::AppointmentDialog(const Patientes &donante, QWidget *parent)
    : QDialog(parent),
      m_service(new AppointmentService(this)),
      m_typeService(new TypeDonationService(this)),
      m_donorService(new DonorService(this)),
      m_donante(donante)
{
    setWindowTitle(QStringLiteral("Dar turno"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    connect(m_service, &AppointmentService::createSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Turno creado correctamente"), Toast::Type::Success);
        emit created();
        accept();
    });
    connect(m_service, &AppointmentService::failed, this, &AppointmentDialog::onFailed);
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

        const auto type = static_cast<AppointmentType>(m_type->currentData().toInt());
        const int typeDonationId = m_donationType->currentData().toInt();
        const std::optional<int> donationTypeId =
            (type == AppointmentType::DONATION && typeDonationId >= 0)
                ? std::optional(typeDonationId)
                : std::nullopt;

        QString seriesAction;
        if (type == AppointmentType::DONATION && m_hasOpenCycle) {
            seriesAction = m_seriesNew->isChecked() ? QStringLiteral("new")
                                                    : QStringLiteral("continue");
        }

        m_service->create(CreateAppointmentDTO(
            m_pendingDatetime,
            type,
            m_donante.id(),
            m_observation->text().trimmed(),
            donationTypeId,
            seriesAction));
    });

    connect(m_typeService, &TypeDonationService::fetched,
        this, &AppointmentDialog::onDonationTypes);
    connect(m_typeService, &TypeDonationService::failed, this, [](const ApiError &) {
        // Sin tipos disponibles: el combo queda con "Sin especificar".
    });

    connect(m_donorService, &DonorService::cyclesFetched,
        this, &AppointmentDialog::onCycles);

    m_typeService->fetchActive();
    m_donorService->fetchCycles(m_donante.id());
}

void AppointmentDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("Dar turno a %1 %2")
        .arg(m_donante.name(), m_donante.lastname()), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(
        QStringLiteral("%1 %2 · Grupo %3")
            .arg(
                m_donante.documentType(),
                m_donante.documentNumber(),
                m_donante.bloodGroup().has_value() ? ::toString(*m_donante.bloodGroup()) : QStringLiteral("—")),
        this);
    subtitle->setProperty("role", "hint");

    auto *form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);
    m_form = form;

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
    m_date->setDate(QDate::currentDate().addDays(1));
    m_date->setMinimumWidth(280);

    m_time = new QComboBox(dateRow);
    m_time->setMinimumWidth(110);
    for (int hour = 8; hour <= 19; ++hour) {
        m_time->addItem(QStringLiteral("%1:00").arg(hour, 2, 10, QLatin1Char('0')));
        if (hour < 19) {
            m_time->addItem(QStringLiteral("%1:30").arg(hour, 2, 10, QLatin1Char('0')));
        }
    }
    m_time->setCurrentText(QStringLiteral("09:00"));

    dateLayout->addWidget(m_date, 1);
    dateLayout->addWidget(m_time);

    m_type = new QComboBox(this);
    m_type->addItem(QStringLiteral("Turno de donación"), static_cast<int>(AppointmentType::DONATION));
    m_type->addItem(QStringLiteral("Control médico"), static_cast<int>(AppointmentType::CHECK));

    m_donationType = new QComboBox(this);
    m_donationType->addItem(QStringLiteral("Sin especificar"), -1);

    m_observation = new QLineEdit(this);
    m_observation->setPlaceholderText(QStringLiteral("Observación (opcional)"));

    m_seriesRow = new QWidget(this);
    auto *seriesLayout = new QHBoxLayout(m_seriesRow);
    seriesLayout->setContentsMargins(0, 0, 0, 0);
    seriesLayout->setSpacing(16);
    m_seriesContinue = new QRadioButton(QStringLiteral("Continuar ciclo actual"), m_seriesRow);
    m_seriesNew = new QRadioButton(QStringLiteral("Iniciar serie nueva"), m_seriesRow);
    m_seriesContinue->setChecked(true);
    seriesLayout->addWidget(m_seriesContinue);
    seriesLayout->addWidget(m_seriesNew);
    seriesLayout->addStretch();

    form->addRow(caption(QStringLiteral("Fecha *")), m_date);
    form->addRow(caption(QStringLiteral("Hora *")), m_time);
    form->addRow(caption(QStringLiteral("Tipo de turno")), m_type);
    form->addRow(caption(QStringLiteral("Tipo de donación")), m_donationType);
    form->addRow(caption(QStringLiteral("Serie de donaciones")), m_seriesRow);
    form->addRow(caption(QStringLiteral("Observación")), m_observation);

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(QStringLiteral("Crear turno"), QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addLayout(form);
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(m_type, &QComboBox::currentIndexChanged, this, &AppointmentDialog::onTypeChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &AppointmentDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &AppointmentDialog::reject);

    onTypeChanged(m_type->currentIndex());
}

void AppointmentDialog::onTypeChanged(int index)
{
    const bool donation = index == 0;
    m_form->setRowVisible(m_donationType, donation);
    if (!donation) {
        m_donationType->setCurrentIndex(0);
    }
    m_form->setRowVisible(m_seriesRow, donation && m_hasOpenCycle);
}

void AppointmentDialog::onCycles(const QJsonObject &page)
{
    const QJsonArray items = page.value(QStringLiteral("data")).toArray();
    m_hasOpenCycle = false;
    for (const QJsonValue &value : items) {
        if (jsonutils::stringOr(value.toObject(), QStringLiteral("state"))
            == QStringLiteral("open")) {
            m_hasOpenCycle = true;
            break;
        }
    }

    const bool donation = m_type->currentIndex() == 0;
    m_form->setRowVisible(m_seriesRow, donation && m_hasOpenCycle);
}

void AppointmentDialog::onDonationTypes(const QList<TypeDonationItem> &items)
{
    m_donationType->clear();
    m_donationType->addItem(QStringLiteral("Sin especificar"), -1);
    for (const TypeDonationItem &item : items) {
        m_donationType->addItem(item.name, item.id);
    }
}

void AppointmentDialog::submit()
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
    m_service->checkAvailability(when);
}

void AppointmentDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    m_date->setEnabled(true);
    m_time->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo crear el turno")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
