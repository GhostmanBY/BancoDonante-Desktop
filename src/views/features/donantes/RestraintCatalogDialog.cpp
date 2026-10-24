#include "views/features/donantes/RestraintCatalogDialog.hpp"

#include "models/enums/UserRole.hpp"
#include "services/RestraintService.hpp"
#include "services/SessionManager.hpp"
#include "views/components/Toast.hpp"
#include "views/features/donantes/RestraintDialog.hpp"
#include "views/features/donantes/RestraintTemplateDialog.hpp"

#include <QBrush>
#include <QColor>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

RestraintCatalogDialog::RestraintCatalogDialog(const Patientes &donante,
                                               QWidget *parent)
    : QDialog(parent),
      m_service(new RestraintService(this)),
      m_donante(donante)
{
    setWindowTitle(QStringLiteral("Restricciones — catálogo"));
    setModal(true);
    setMinimumSize(640, 420);

    buildUi();

    connect(m_service, &RestraintService::templatesFetched, this,
            &RestraintCatalogDialog::onTemplatesFetched);
    connect(m_service, &RestraintService::failed, this,
            &RestraintCatalogDialog::onFailed);

    reload();
}

void RestraintCatalogDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        QStringLiteral("Restricciones para %1 %2")
            .arg(m_donante.name(), m_donante.lastname()),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    m_hint = new QLabel(
        QStringLiteral("El catálogo es global. Seleccioná una restricción para "
                       "asignarla (se copia al donante con su rango por "
                       "defecto, editable antes de confirmar), o agregá y "
                       "editá entradas del catálogo."),
        this);
    m_hint->setWordWrap(true);
    m_hint->setProperty("role", "hint");

    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels(
        {QStringLiteral("Motivo"), QStringLiteral("Duración (default)"),
         QStringLiteral("Estado")});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);

    m_formError = new QLabel(this);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *actionRow = new QHBoxLayout();
    m_assignBtn = new QPushButton(QStringLiteral("Asignar al donante…"), this);
    m_assignBtn->setProperty("variant", "primary");
    m_manualBtn = new QPushButton(QStringLiteral("Asignar sin plantilla…"), this);
    m_newBtn = new QPushButton(QStringLiteral("Nueva…"), this);
    m_editBtn = new QPushButton(QStringLiteral("Editar…"), this);
    actionRow->addWidget(m_assignBtn);
    actionRow->addWidget(m_manualBtn);
    actionRow->addStretch();
    actionRow->addWidget(m_newBtn);
    actionRow->addWidget(m_editBtn);

    auto *buttons = new QDialogButtonBox(this);
    buttons->addButton(QStringLiteral("Cerrar"), QDialogButtonBox::RejectRole);

    layout->addWidget(title);
    layout->addWidget(m_hint);
    layout->addWidget(m_table, 1);
    layout->addWidget(m_formError);
    layout->addLayout(actionRow);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::rejected, this, &RestraintCatalogDialog::reject);

    connect(m_assignBtn, &QPushButton::clicked, this,
            &RestraintCatalogDialog::openAssign);
    connect(m_manualBtn, &QPushButton::clicked, this,
            &RestraintCatalogDialog::openManualAssign);
    connect(m_newBtn, &QPushButton::clicked, this,
            &RestraintCatalogDialog::openCreate);
    connect(m_editBtn, &QPushButton::clicked, this,
            &RestraintCatalogDialog::openEdit);
    connect(m_table, &QTableWidget::itemSelectionChanged, this,
            &RestraintCatalogDialog::updateButtons);

    const bool manage = canManage();
    m_newBtn->setEnabled(manage);
    m_editBtn->setEnabled(manage);
    m_manualBtn->setEnabled(manage);

    updateButtons();
}

void RestraintCatalogDialog::reload()
{
    m_formError->clear();
    m_formError->setVisible(false);
    m_service->fetchTemplates();
}

void RestraintCatalogDialog::onTemplatesFetched(
    const QList<RestraintTemplateItem> &templates)
{
    m_templates = templates;
    m_table->setRowCount(templates.size());

    for (int row = 0; row < templates.size(); ++row) {
        const RestraintTemplateItem &item = templates.at(row);

        auto *motivo = new QTableWidgetItem(item.motivo);
        if (!item.isActive) {
            motivo->setForeground(QBrush(QColor(QStringLiteral("#9ca3af"))));
        }
        m_table->setItem(row, 0, motivo);
        m_table->setItem(row, 1, new QTableWidgetItem(
            item.defaultMonths == 1
                ? QStringLiteral("1 mes")
                : QStringLiteral("%1 meses").arg(item.defaultMonths)));
        m_table->setItem(row, 2, new QTableWidgetItem(
            item.isActive ? QStringLiteral("Activa")
                          : QStringLiteral("Inactiva")));
    }

    if (templates.size() > 0) {
        m_table->selectRow(0);
    }
    updateButtons();
}

const RestraintTemplateItem *RestraintCatalogDialog::selectedTemplate() const
{
    const int row = m_table->currentRow();
    if (row < 0 || row >= m_templates.size()) {
        return nullptr;
    }
    return &m_templates.at(row);
}

void RestraintCatalogDialog::updateButtons()
{
    const RestraintTemplateItem *selected = selectedTemplate();
    m_assignBtn->setEnabled(selected != nullptr && selected->isActive);
    m_editBtn->setEnabled(canManage() && selected != nullptr);
}

void RestraintCatalogDialog::openAssign()
{
    const RestraintTemplateItem *selected = selectedTemplate();
    if (!selected) {
        m_formError->setText(QStringLiteral("Seleccioná una restricción del catálogo."));
        m_formError->setVisible(true);
        return;
    }
    if (!selected->isActive) {
        m_formError->setText(
            QStringLiteral("La restricción seleccionada está inactiva. "
                           "Activala o elegí otra."));
        m_formError->setVisible(true);
        return;
    }

    auto *dialog = new RestraintDialog(m_donante, *selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RestraintDialog::assigned, this,
            [this](int cancelled) {
                emit assigned(cancelled);
                accept();
            });
    dialog->open();
}

void RestraintCatalogDialog::openManualAssign()
{
    auto *dialog = new RestraintDialog(m_donante, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RestraintDialog::assigned, this,
            [this](int cancelled) {
                emit assigned(cancelled);
                accept();
            });
    dialog->open();
}

void RestraintCatalogDialog::openCreate()
{
    auto *dialog = new RestraintTemplateDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RestraintTemplateDialog::saved, this,
            &RestraintCatalogDialog::reload);
    dialog->open();
}

void RestraintCatalogDialog::openEdit()
{
    const RestraintTemplateItem *selected = selectedTemplate();
    if (!selected) {
        m_formError->setText(QStringLiteral("Seleccioná una restricción del catálogo."));
        m_formError->setVisible(true);
        return;
    }

    auto *dialog = new RestraintTemplateDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RestraintTemplateDialog::saved, this,
            &RestraintCatalogDialog::reload);
    dialog->open();
}

bool RestraintCatalogDialog::canManage() const
{
    switch (SessionManager::instance().user().role) {
    case UserRole::ADMIN:
    case UserRole::MEDICAL:
    case UserRole::AREA_MANAGER:
        return true;
    case UserRole::NURSE:
        return false;
    }
    return false;
}

void RestraintCatalogDialog::onFailed(const ApiError &error)
{
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo cargar el catálogo de restricciones")
        : error.detail;
    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
