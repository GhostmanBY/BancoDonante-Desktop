#include "views/features/donantes/RestraintDialog.hpp"

#include "dtos/Restraints/CreateRestraintDTO.hpp"
#include "exceptions/Validation/ValidationException.hpp"
#include "services/RestraintService.hpp"
#include "views/components/Toast.hpp"

#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RestraintDialog::RestraintDialog(const Patientes &donante, QWidget *parent)
    : QDialog(parent),
      m_service(new RestraintService(this)),
      m_donante(donante)
{
    setWindowTitle(QStringLiteral("Asignar restricción"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    connect(m_service, &RestraintService::restraintAssigned, this,
        [this](int cancelled) {
            Toast::show(
                this,
                cancelled > 0
                    ? QStringLiteral("Restricción asignada · %1 turno(s) cancelado(s)")
                          .arg(cancelled)
                    : QStringLiteral("Restricción asignada correctamente"),
                Toast::Type::Success);
            emit assigned(cancelled);
            accept();
        });
    connect(m_service, &RestraintService::failed, this, &RestraintDialog::onFailed);
}

RestraintDialog::RestraintDialog(
    const Patientes &donante,
    const RestraintTemplateItem &prefill,
    QWidget *parent)
    : QDialog(parent),
      m_service(new RestraintService(this)),
      m_donante(donante),
      m_templateId(prefill.id)
{
    setWindowTitle(QStringLiteral("Asignar restricción"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    // El período se calcula desde la fecha de hoy: desde = hoy,
    // hasta = hoy + duración de la plantilla (QDate::addMonths
    // maneja fin de mes: 31/01 + 1 = 28/02). Editable antes de
    // confirmar; las fechas efectivas se persisten en la asignación.
    const QDate today = QDate::currentDate();
    m_motivo->setText(prefill.motivo);
    m_startDate->setDate(today);
    m_endDate->setDate(today.addMonths(prefill.defaultMonths));

    connect(m_service, &RestraintService::restraintAssigned, this,
        [this](int cancelled) {
            Toast::show(
                this,
                cancelled > 0
                    ? QStringLiteral("Restricción asignada · %1 turno(s) cancelado(s)")
                          .arg(cancelled)
                    : QStringLiteral("Restricción asignada correctamente"),
                Toast::Type::Success);
            emit assigned(cancelled);
            accept();
        });
    connect(m_service, &RestraintService::failed, this, &RestraintDialog::onFailed);
}

void RestraintDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        QStringLiteral("Restricción para %1 %2")
            .arg(m_donante.name(), m_donante.lastname()),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *hint = new QLabel(
        QStringLiteral("Mientras la restricción esté vigente, el donante no podrá donar. "
                       "Los turnos pendientes o confirmados dentro del período se "
                       "cancelarán automáticamente y el donante pasará a estado "
                       "«no apto»."),
        this);
    hint->setWordWrap(true);
    hint->setProperty("role", "hint");

    auto *form = new QFormLayout();
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_motivo = new QLineEdit(this);
    m_motivo->setMaxLength(155);
    m_motivo->setPlaceholderText(QStringLiteral("Ej.: Medicación temporal"));
    form->addRow(QStringLiteral("Motivo *"), m_motivo);

    m_startDate = new QDateEdit(this);
    m_startDate->setCalendarPopup(true);
    m_startDate->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_startDate->setDate(QDate::currentDate());
    form->addRow(QStringLiteral("Desde *"), m_startDate);

    m_endDate = new QDateEdit(this);
    m_endDate->setCalendarPopup(true);
    m_endDate->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_endDate->setDate(QDate::currentDate().addDays(14));
    form->addRow(QStringLiteral("Hasta *"), m_endDate);

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(
        QStringLiteral("Asignar restricción"), QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(hint);
    layout->addLayout(form);
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &RestraintDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &RestraintDialog::reject);
}

bool RestraintDialog::validate()
{
    m_formError->clear();
    m_formError->setVisible(false);

    const QString motivo = m_motivo->text().trimmed();
    if (motivo.isEmpty()) {
        m_formError->setText(QStringLiteral("El motivo es obligatorio."));
        m_formError->setVisible(true);
        return false;
    }

    const QDate start = m_startDate->date();
    const QDate end = m_endDate->date();

    if (start < QDate::currentDate()) {
        m_formError->setText(QStringLiteral("La fecha de inicio no puede ser en el pasado."));
        m_formError->setVisible(true);
        return false;
    }

    if (end < start) {
        m_formError->setText(QStringLiteral("La fecha de fin no puede ser anterior a la de inicio."));
        m_formError->setVisible(true);
        return false;
    }

    return true;
}

void RestraintDialog::submit()
{
    if (!validate()) {
        return;
    }

    m_acceptBtn->setEnabled(false);

    try {
        const CreateRestraintDTO dto(
            m_motivo->text().trimmed(),
            m_startDate->date().startOfDay(),
            m_endDate->date().startOfDay(),
            m_templateId);
        m_service->assignRestraint(m_donante.id(), dto);
    } catch (const ValidationException &e) {
        m_acceptBtn->setEnabled(true);
        m_formError->setText(e.message());
        m_formError->setVisible(true);
    }
}

void RestraintDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo asignar la restricción")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
