#include "views/features/donantes/RestraintTemplateDialog.hpp"

#include "dtos/Restraints/CreateRestraintTemplateDTO.hpp"
#include "dtos/Restraints/UpdateRestraintTemplateDTO.hpp"
#include "exceptions/Validation/ValidationException.hpp"
#include "services/RestraintService.hpp"
#include "views/components/Toast.hpp"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

RestraintTemplateDialog::RestraintTemplateDialog(QWidget *parent)
    : QDialog(parent),
      m_service(new RestraintService(this))
{
    setWindowTitle(QStringLiteral("Nueva restricción (catálogo)"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    connect(m_service, &RestraintService::templateSaved, this, [this](int) {
        Toast::show(this, QStringLiteral("Restricción agregada al catálogo"),
                    Toast::Type::Success);
        emit saved();
        accept();
    });
    connect(m_service, &RestraintService::failed, this,
            &RestraintTemplateDialog::onFailed);
}

RestraintTemplateDialog::RestraintTemplateDialog(
    const RestraintTemplateItem &item, QWidget *parent)
    : QDialog(parent),
      m_service(new RestraintService(this)),
      m_templateId(item.id)
{
    setWindowTitle(QStringLiteral("Editar restricción del catálogo"));
    setModal(true);
    setMinimumWidth(480);

    buildUi();

    m_motivo->setText(item.motivo);
    m_months->setValue(item.defaultMonths);

    connect(m_service, &RestraintService::templateSaved, this, [this](int) {
        Toast::show(this, QStringLiteral("Restricción del catálogo actualizada"),
                    Toast::Type::Success);
        emit saved();
        accept();
    });
    connect(m_service, &RestraintService::failed, this,
            &RestraintTemplateDialog::onFailed);
}

void RestraintTemplateDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        m_templateId > 0
            ? QStringLiteral("Editar restricción del catálogo")
            : QStringLiteral("Nueva restricción en el catálogo"),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *hint = new QLabel(
        QStringLiteral("El catálogo es global: la restricción queda disponible "
                       "para asignarla a cualquier donante. Al asignarla se "
                       "calcula el período desde la fecha de hoy más esta "
                       "duración (editable) y se copia al donante sin "
                       "modificar este registro."),
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

    m_months = new QSpinBox(this);
    m_months->setRange(1, 120);
    m_months->setSuffix(QStringLiteral(" mes(es)"));
    m_months->setValue(1);
    form->addRow(QStringLiteral("Duración (default) *"), m_months);

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(
        m_templateId > 0 ? QStringLiteral("Guardar cambios")
                         : QStringLiteral("Agregar al catálogo"),
        QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(hint);
    layout->addLayout(form);
    layout->addWidget(m_formError);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this,
            &RestraintTemplateDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this,
            &RestraintTemplateDialog::reject);
}

bool RestraintTemplateDialog::validate()
{
    m_formError->clear();
    m_formError->setVisible(false);

    if (m_motivo->text().trimmed().isEmpty()) {
        m_formError->setText(QStringLiteral("El motivo es obligatorio."));
        m_formError->setVisible(true);
        return false;
    }

    if (m_months->value() < 1) {
        m_formError->setText(QStringLiteral("La duración debe ser al menos 1 mes."));
        m_formError->setVisible(true);
        return false;
    }

    return true;
}

void RestraintTemplateDialog::submit()
{
    if (!validate()) {
        return;
    }

    m_acceptBtn->setEnabled(false);

    try {
        if (m_templateId > 0) {
            m_service->updateTemplate(
                m_templateId,
                UpdateRestraintTemplateDTO(
                    m_templateId,
                    m_motivo->text().trimmed(),
                    m_months->value()));
        } else {
            m_service->createTemplate(CreateRestraintTemplateDTO(
                m_motivo->text().trimmed(),
                m_months->value()));
        }
    } catch (const ValidationException &e) {
        m_acceptBtn->setEnabled(true);
        m_formError->setText(e.message());
        m_formError->setVisible(true);
    }
}

void RestraintTemplateDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo guardar la restricción del catálogo")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
