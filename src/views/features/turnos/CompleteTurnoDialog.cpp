#include "views/features/turnos/CompleteTurnoDialog.hpp"

#include "dtos/Appointments/UpdateAppointmentDTO.hpp"
#include "views/components/Toast.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

CompleteTurnoDialog::CompleteTurnoDialog(const TurnoItem &item, QWidget *parent)
    : QDialog(parent),
      m_item(item),
      m_service(new AppointmentService(this))
{
    setWindowTitle(QStringLiteral("Completar turno"));
    setModal(true);
    setMinimumWidth(540);

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

    m_clinicalDataGroup = new QGroupBox(
        QStringLiteral("Registrar datos clínicos"), this);
    m_clinicalDataGroup->setCheckable(true);
    m_clinicalDataGroup->setChecked(false);
    auto *clinicalForm = new QFormLayout(m_clinicalDataGroup);

    auto makeComponentCount = [this, clinicalForm](const QString &label) {
        auto *count = new QSpinBox(m_clinicalDataGroup);
        count->setRange(0, 3);
        clinicalForm->addRow(label, count);
        return count;
    };
    m_wholeBloodComponent = makeComponentCount(QStringLiteral("Sangre entera (unidades)"));
    m_plateletsComponent = makeComponentCount(QStringLiteral("Plaquetas (unidades)"));
    m_plasmaComponent = makeComponentCount(QStringLiteral("Plasma (unidades)"));

    m_plateletsPerUl = new QSpinBox(m_clinicalDataGroup);
    m_plateletsPerUl->setRange(0, 1000000000);
    m_plateletsPerUl->setGroupSeparatorShown(true);
    clinicalForm->addRow(QStringLiteral("Plaquetas (por µL)"), m_plateletsPerUl);

    auto makeMeasurement = [this](int decimals) {
        auto *spinBox = new QDoubleSpinBox(m_clinicalDataGroup);
        spinBox->setRange(0.0, 1000000.0);
        spinBox->setDecimals(decimals);
        return spinBox;
    };
    m_hemoglobin = makeMeasurement(2);
    clinicalForm->addRow(QStringLiteral("Hemoglobina (g/dL)"), m_hemoglobin);
    m_redBloodCells = makeMeasurement(2);
    clinicalForm->addRow(QStringLiteral("Glóbulos rojos (millones/µL)"), m_redBloodCells);
    m_hematocrit = makeMeasurement(2);
    m_hematocrit->setRange(0.0, 100.0);
    clinicalForm->addRow(QStringLiteral("Hematocrito (%)"), m_hematocrit);

    m_bagType = new QLineEdit(m_clinicalDataGroup);
    m_bagType->setMaxLength(80);
    m_bagType->setPlaceholderText(QStringLiteral("Ej.: Triple"));
    clinicalForm->addRow(QStringLiteral("Tipo de bolsa"), m_bagType);

    m_arm = new QComboBox(m_clinicalDataGroup);
    m_arm->addItem(QStringLiteral("Izquierdo"), QStringLiteral("left"));
    m_arm->addItem(QStringLiteral("Derecho"), QStringLiteral("right"));
    clinicalForm->addRow(QStringLiteral("Brazo"), m_arm);

    const bool canRecordClinicalData = m_item.type == AppointmentType::DONATION
        && m_item.typeDonationId.has_value();
    m_clinicalDataGroup->setVisible(canRecordClinicalData);
    auto *clinicalDataHint = new QLabel(
        QStringLiteral("Para registrar datos clínicos, el turno debe tener un tipo de donación."),
        this);
    clinicalDataHint->setProperty("role", "hint");
    clinicalDataHint->setWordWrap(true);
    clinicalDataHint->setVisible(
        m_item.type == AppointmentType::DONATION && !m_item.typeDonationId.has_value());

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
    if (canRecordClinicalData) {
        layout->addWidget(m_clinicalDataGroup);
    } else if (m_item.type == AppointmentType::DONATION) {
        layout->addWidget(clinicalDataHint);
    }
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &CompleteTurnoDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &CompleteTurnoDialog::reject);
}

void CompleteTurnoDialog::submit()
{
    m_formError->setVisible(false);

    QJsonObject clinicalData;
    if (m_item.type == AppointmentType::DONATION
        && m_item.typeDonationId.has_value()) {
        if (m_clinicalDataGroup->isChecked()) {
            if (m_bagType->text().trimmed().isEmpty()) {
                m_acceptBtn->setEnabled(true);
                m_formError->setText(QStringLiteral("El tipo de bolsa es obligatorio para guardar los datos clínicos."));
                m_formError->setVisible(true);
                return;
            }

            clinicalData.insert(QStringLiteral("whole_blood_component"), m_wholeBloodComponent->value());
            clinicalData.insert(QStringLiteral("platelets_component"), m_plateletsComponent->value());
            clinicalData.insert(QStringLiteral("plasma_component"), m_plasmaComponent->value());
            clinicalData.insert(QStringLiteral("platelets_per_ul"), m_plateletsPerUl->value());
            clinicalData.insert(QStringLiteral("hemoglobin_g_dl"), m_hemoglobin->value());
            clinicalData.insert(QStringLiteral("red_blood_cells_million_per_ul"), m_redBloodCells->value());
            clinicalData.insert(QStringLiteral("hematocrit_percent"), m_hematocrit->value());
            clinicalData.insert(QStringLiteral("bag_type"), m_bagType->text().trimmed());
            clinicalData.insert(QStringLiteral("arm"), m_arm->currentData().toString());
        }
    }

    m_acceptBtn->setEnabled(false);
    m_service->update(UpdateAppointmentDTO(
        m_item.id,
        m_item.datetime,
        AppointmentState::COMPLETED,
        m_item.type,
        m_item.patientId,
        m_observation->text().trimmed(),
        clinicalData));
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
