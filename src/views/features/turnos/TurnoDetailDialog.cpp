#include "views/features/turnos/TurnoDetailDialog.hpp"

#include "core/JsonUtils.hpp"
#include "models/enums/AppointmentState.hpp"
#include "views/components/Toast.hpp"

#include <QDateTime>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>

TurnoDetailDialog::TurnoDetailDialog(int appointmentId, QWidget *parent)
    : QDialog(parent),
      m_service(new AppointmentService(this))
{
    setWindowTitle(QStringLiteral("Detalle del turno"));
    setModal(true);
    setMinimumWidth(500);

    buildUi();

    connect(m_service, &AppointmentService::appointmentFetched,
        this, &TurnoDetailDialog::onAppointmentFetched);
    connect(m_service, &AppointmentService::failed,
        this, &TurnoDetailDialog::onFailed);
    m_service->fetchById(appointmentId);
}

void TurnoDetailDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("Datos del turno"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));
    m_loading = new QLabel(QStringLiteral("Cargando…"), this);
    m_loading->setProperty("role", "hint");

    auto *content = new QWidget(this);
    m_form = new QFormLayout(content);
    m_form->setSpacing(10);
    m_form->setLabelAlignment(Qt::AlignLeft);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &TurnoDetailDialog::reject);

    layout->addWidget(title);
    layout->addWidget(m_loading);
    layout->addWidget(content);
    layout->addWidget(buttons, 0, Qt::AlignRight);
}

void TurnoDetailDialog::onAppointmentFetched(const QJsonObject &appointment)
{
    m_loading->hide();

    const QDateTime datetime = jsonutils::parseDateTime(
        jsonutils::stringOr(appointment, QStringLiteral("appointment_datetime")));
    addRow(QStringLiteral("ID"), QString::number(jsonutils::intOr(
        appointment, QStringLiteral("id"))));
    addRow(QStringLiteral("ID del paciente"), QString::number(jsonutils::intOr(
        appointment, QStringLiteral("patient_id"))));
    addRow(QStringLiteral("Paciente"),
        QStringLiteral("%1 %2")
            .arg(jsonutils::stringOr(appointment, QStringLiteral("patient_name")),
                 jsonutils::stringOr(appointment, QStringLiteral("patient_lastname"))));
    addRow(QStringLiteral("Fecha y hora"), datetime.isValid()
        ? QLocale().toString(datetime.toLocalTime(), QStringLiteral("dddd d 'de' MMMM 'de' yyyy, HH:mm"))
        : QStringLiteral("—"));

    const QString type = jsonutils::stringOr(appointment, QStringLiteral("type"));
    addRow(QStringLiteral("Tipo"), type == QStringLiteral("donation")
        ? QStringLiteral("Turno de donación")
        : QStringLiteral("Control médico"));
    addRow(QStringLiteral("Estado"), appointmentStateLabel(appointmentStateFromString(
        jsonutils::stringOr(appointment, QStringLiteral("state")),
        AppointmentState::PENDING)));
    addRow(QStringLiteral("Responsable asignado"),
        jsonutils::stringOr(appointment, QStringLiteral("assigned_users")).isEmpty()
            ? QStringLiteral("Sin asignar")
            : jsonutils::stringOr(appointment, QStringLiteral("assigned_users")));
    addRow(QStringLiteral("Observación"),
        jsonutils::stringOr(appointment, QStringLiteral("observation"), QStringLiteral("—")));

    const QJsonValue donationType = appointment.value(QStringLiteral("type_donation_id"));
    addRow(QStringLiteral("Tipo de donación"), donationType.isNull()
        ? QStringLiteral("No especificado")
        : QString::number(donationType.toInt()));
    const QString seriesAction = jsonutils::stringOr(
        appointment, QStringLiteral("series_action"));
    addRow(QStringLiteral("Acción de serie"), seriesAction == QStringLiteral("new")
        ? QStringLiteral("Iniciar nueva serie")
        : seriesAction == QStringLiteral("continue")
            ? QStringLiteral("Continuar serie")
            : QStringLiteral("Predeterminada"));

    addClinicalData(appointment.value(QStringLiteral("clinical_data")));
    adjustSize();
}

void TurnoDetailDialog::addRow(const QString &label, const QString &value)
{
    auto *caption = new QLabel(label, this);
    caption->setProperty("role", "hint");
    auto *content = new QLabel(value.isEmpty() ? QStringLiteral("—") : value, this);
    content->setWordWrap(true);
    m_form->addRow(caption, content);
}

void TurnoDetailDialog::addClinicalData(const QJsonValue &value)
{
    if (!value.isObject()) {
        addRow(QStringLiteral("Datos clínicos"), QStringLiteral("Sin datos clínicos registrados"));
        return;
    }

    const QJsonObject data = value.toObject();
    auto componentCount = [&data](const QString &field) {
        return QString::number(jsonutils::intOr(data, field));
    };
    addRow(QStringLiteral("Sangre entera (unidades)"),
        componentCount(QStringLiteral("whole_blood_component")));
    addRow(QStringLiteral("Plaquetas (unidades)"),
        componentCount(QStringLiteral("platelets_component")));
    addRow(QStringLiteral("Plasma (unidades)"),
        componentCount(QStringLiteral("plasma_component")));
    addRow(QStringLiteral("Plaquetas (por µL)"),
        QString::number(jsonutils::intOr(data, QStringLiteral("platelets_per_ul"))));
    addRow(QStringLiteral("Hemoglobina (g/dL)"),
        QString::number(jsonutils::optDouble(
            data, QStringLiteral("hemoglobin_g_dl")).value_or(0.0)));
    addRow(QStringLiteral("Glóbulos rojos (millones/µL)"),
        QString::number(jsonutils::optDouble(
            data, QStringLiteral("red_blood_cells_million_per_ul")).value_or(0.0)));
    addRow(QStringLiteral("Hematocrito (%)"),
        QString::number(jsonutils::optDouble(
            data, QStringLiteral("hematocrit_percent")).value_or(0.0)));
    addRow(QStringLiteral("Tipo de bolsa"),
        jsonutils::stringOr(data, QStringLiteral("bag_type")));
    const QString arm = jsonutils::stringOr(data, QStringLiteral("arm"));
    addRow(QStringLiteral("Brazo"), arm == QStringLiteral("left")
        ? QStringLiteral("Izquierdo")
        : arm == QStringLiteral("right")
            ? QStringLiteral("Derecho")
            : QStringLiteral("—"));
}

void TurnoDetailDialog::onFailed(const ApiError &error)
{
    m_loading->setText(error.detail.isEmpty()
        ? QStringLiteral("No se pudieron cargar los datos del turno")
        : error.detail);
    Toast::show(this, m_loading->text(), Toast::Type::Error);
}
