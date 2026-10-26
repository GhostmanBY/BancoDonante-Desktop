#include "views/features/donantes/DonanteDetailDialog.hpp"

#include "core/JsonUtils.hpp"
#include "models/enums/BloodGroup.hpp"
#include "models/enums/PatientState.hpp"
#include "models/enums/UserRole.hpp"
#include "models/Restraint.hpp"
#include "services/DonorService.hpp"
#include "services/RestraintService.hpp"
#include "services/SessionManager.hpp"
#include "views/components/Toast.hpp"
#include "views/features/donantes/AddressDialog.hpp"

#include <QDialogButtonBox>
#include <QDate>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QStringList>
#include <QVBoxLayout>

namespace {

QString estadoLabel(PatientState state)
{
    switch (state) {
    case PatientState::APTO: return QStringLiteral("Apto");
    case PatientState::REVISON: return QStringLiteral("En revisión");
    case PatientState::NO_APTO: return QStringLiteral("No apto");
    case PatientState::SUSPENDED: return QStringLiteral("Suspendido");
    }
    return {};
}

QLabel *hinted(const QString &value, QWidget *parent)
{
    auto *label = new QLabel(value.isEmpty() ? QStringLiteral("—") : value, parent);
    return label;
}

QString haceDias(const QDate &date)
{
    const int days = qAbs(QDate::currentDate().daysTo(date));
    if (days == 0) {
        return QStringLiteral("hoy");
    }
    return QStringLiteral("hace %1 día%2")
        .arg(days)
        .arg(days == 1 ? QString() : QStringLiteral("s"));
}

} // namespace

DonanteDetailDialog::DonanteDetailDialog(
    const Patientes &donante,
    QWidget *parent)
    : QDialog(parent),
      m_service(new DonorService(this)),
      m_restraintService(new RestraintService(this)),
      m_ruleService(new CycleRuleService(this)),
      m_donante(donante)
{
    setWindowTitle(QStringLiteral("Detalle del donante"));
    setModal(true);
    resize(760, 560);

    buildUi();
    updateToggleLabel();

    connect(m_service, &DonorService::addressesFetched, this, &DonanteDetailDialog::onAddresses);
    connect(m_service, &DonorService::cyclesFetched, this, &DonanteDetailDialog::onCycles);
    connect(m_service, &DonorService::cycleDetailFetched, this, &DonanteDetailDialog::onCycleDetail);
    connect(m_service, &DonorService::addressCurrentSet, this, [this]() {
        Toast::show(this, QStringLiteral("Dirección marcada como vigente"), Toast::Type::Success);
        refreshAddresses();
    });
    connect(m_service, &DonorService::addressDeleted, this, [this]() {
        Toast::show(this, QStringLiteral("Dirección eliminada"), Toast::Type::Success);
        refreshAddresses();
    });
    connect(m_service, &DonorService::failed, this, &DonanteDetailDialog::onFailed);

    connect(m_restraintService, &RestraintService::observationsFetched,
        this, &DonanteDetailDialog::onObservations);
    connect(m_restraintService, &RestraintService::restraintsFetched,
        this, &DonanteDetailDialog::onRestraints);
    connect(m_restraintService, &RestraintService::failed,
        this, &DonanteDetailDialog::onFailed);

    connect(m_ruleService, &CycleRuleService::fetched, this,
        [this](const QList<CycleRuleItem> &rules) {
            m_cycleRules = rules;
            updateLastDonationLabelColor();
        });

    connect(m_toggleBtn, &QPushButton::clicked, this, [this]() {
        m_toggleBtn->setEnabled(false);
        emit toggleRequested(m_donante.id(), !m_donante.isActive());
    });

    refreshAddresses();
    refreshCycles();
    refreshObservaciones();
    refreshRestricciones();
    m_ruleService->fetchAll();
}

void DonanteDetailDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 14);
    layout->setSpacing(12);

    auto *title = new QLabel(
        QStringLiteral("%1 %2").arg(m_donante.name(), m_donante.lastname()), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 18px; font-weight: 700; color: #111827; }"));

    auto *subtitle = new QLabel(
        QStringLiteral("%1 %2 · %3")
            .arg(m_donante.documentType(), m_donante.documentNumber(), m_donante.mail()),
        this);
    subtitle->setProperty("role", "hint");

    auto *tabs = new QTabWidget(this);
    tabs->addTab(buildDatosTab(), QStringLiteral("Datos"));
    tabs->addTab(buildDireccionesTab(), QStringLiteral("Direcciones"));
    tabs->addTab(buildCiclosTab(), QStringLiteral("Ciclos"));
    tabs->addTab(buildObservacionesTab(), QStringLiteral("Observaciones"));

    auto *closeBtn = new QDialogButtonBox(this);
    m_toggleBtn = closeBtn->addButton(QStringLiteral("Desactivar"), QDialogButtonBox::ActionRole);
    m_toggleBtn->setProperty("variant", "danger");
    closeBtn->addButton(QStringLiteral("Cerrar"), QDialogButtonBox::RejectRole);
    connect(closeBtn, &QDialogButtonBox::rejected, this, &DonanteDetailDialog::reject);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(tabs, 1);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);
}

QWidget *DonanteDetailDialog::buildDatosTab()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);
    form->setContentsMargins(8, 12, 8, 8);
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    auto addRow = [form, page](const QString &label, const QString &value) {
        auto *caption = new QLabel(label, page);
        caption->setProperty("role", "hint");
        form->addRow(caption, hinted(value, page));
    };

    addRow(QStringLiteral("Nombre completo"),
           QStringLiteral("%1 %2").arg(m_donante.name(), m_donante.lastname()));
    addRow(QStringLiteral("Documento"),
           QStringLiteral("%1 %2").arg(m_donante.documentType(), m_donante.documentNumber()));
    addRow(QStringLiteral("Mail"), m_donante.mail());
    addRow(QStringLiteral("Teléfono"), m_donante.phone());
    addRow(QStringLiteral("Fecha de nacimiento"),
           m_donante.birthDate().has_value() && m_donante.birthDate()->isValid()
               ? m_donante.birthDate()->date().toString(QStringLiteral("dd/MM/yyyy"))
               : QString());
    addRow(QStringLiteral("Lugar de nacimiento"), m_donante.birthPlace().value_or(QString()));
    addRow(QStringLiteral("Sexo"), ::toString(m_donante.sex()));
    addRow(QStringLiteral("Grupo sanguíneo"),
           m_donante.bloodGroup().has_value() ? ::toString(*m_donante.bloodGroup()) : QString());
    addRow(QStringLiteral("Peso"),
           m_donante.peso().has_value()
               ? QStringLiteral("%1 kg").arg(*m_donante.peso())
               : QString());
    addRow(QStringLiteral("Altura"),
           m_donante.altura().has_value()
               ? QStringLiteral("%1 cm").arg(*m_donante.altura())
               : QString());

    m_stateLabel = new QLabel(estadoLabel(m_donante.stateDonation()), page);
    m_stateLabel->setStyleSheet(QStringLiteral("font-weight: 600;"));
    form->addRow(QStringLiteral("Estado de donación"), m_stateLabel);

    m_restraintLabel = hinted(QStringLiteral("Cargando…"), page);
    m_restraintLabel->setWordWrap(true);
    form->addRow(QStringLiteral("Restricción"), m_restraintLabel);

    m_lastDonationLabel = new QLabel(
        m_donante.lastDonationDate().has_value() && m_donante.lastDonationDate()->isValid()
            ? QStringLiteral("%1 · %2")
                  .arg(m_donante.lastDonationDate()->toString(QStringLiteral("dd/MM/yyyy HH:mm")),
                       haceDias(m_donante.lastDonationDate()->date()))
            : QStringLiteral("Sin donaciones"),
        page);
    updateLastDonationLabelColor();
    form->addRow(QStringLiteral("Última donación"), m_lastDonationLabel);

    auto *activeLabel = new QLabel(m_donante.isActive() ? QStringLiteral("Sí") : QStringLiteral("No"), page);
    form->addRow(QStringLiteral("Activo"), activeLabel);

    return page;
}

QWidget *DonanteDetailDialog::buildDireccionesTab()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 12, 8, 8);
    layout->setSpacing(8);

    m_addressesTable = new QTableWidget(0, 5, page);
    m_addressesTable->setHorizontalHeaderLabels({
        QStringLiteral("Calle"),
        QStringLiteral("Nro."),
        QStringLiteral("Ciudad"),
        QStringLiteral("Vigente"),
        QStringLiteral("ID"),
    });
    m_addressesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_addressesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_addressesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_addressesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_addressesTable->verticalHeader()->setVisible(false);
    m_addressesTable->setColumnHidden(4, true);

    auto *actions = new QHBoxLayout();
    auto *currentBtn = new QPushButton(QStringLiteral("Marcar vigente"), page);
    auto *deleteBtn = new QPushButton(QStringLiteral("Eliminar"), page);
    deleteBtn->setProperty("variant", "danger");
    auto *addBtn = new QPushButton(QStringLiteral("+ Agregar dirección"), page);

    const UserRole role = SessionManager::instance().user().role;
    const bool canManageAddresses =
        role == UserRole::ADMIN || role == UserRole::MEDICAL || role == UserRole::NURSE;
    addBtn->setVisible(canManageAddresses);

    actions->addWidget(addBtn);
    actions->addWidget(currentBtn);
    actions->addWidget(deleteBtn);
    actions->addStretch();

    layout->addWidget(m_addressesTable);
    layout->addLayout(actions);

    connect(addBtn, &QPushButton::clicked, this, [this]() {
        auto *dialog = new AddressDialog(m_donante.id(), this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, &AddressDialog::saved, this, [this]() {
            refreshAddresses();
        });
        dialog->open();
    });

    connect(currentBtn, &QPushButton::clicked, this, [this]() {
        const int row = m_addressesTable->currentRow();
        if (row < 0) {
            Toast::show(this, QStringLiteral("Elegí una dirección primero"), Toast::Type::Info);
            return;
        }
        m_service->makeCurrentAddress(m_addressesTable->item(row, 4)->text().toInt());
    });
    connect(deleteBtn, &QPushButton::clicked, this, [this]() {
        const int row = m_addressesTable->currentRow();
        if (row < 0) {
            Toast::show(this, QStringLiteral("Elegí una dirección primero"), Toast::Type::Info);
            return;
        }
        m_service->deleteAddress(m_addressesTable->item(row, 4)->text().toInt());
    });

    return page;
}

QWidget *DonanteDetailDialog::buildCiclosTab()
{
    auto *page = new QWidget(this);
    auto *layout = new QHBoxLayout(page);
    layout->setContentsMargins(8, 12, 8, 8);
    layout->setSpacing(8);

    m_cyclesList = new QListWidget(page);
    m_cyclesList->setMinimumWidth(220);

    m_donationsTable = new QTableWidget(0, 3, page);
    m_donationsTable->setHorizontalHeaderLabels({
        QStringLiteral("Nº"),
        QStringLiteral("Fecha del turno"),
        QStringLiteral("Tipo de donación"),
    });
    m_donationsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_donationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_donationsTable->verticalHeader()->setVisible(false);

    layout->addWidget(m_cyclesList);
    layout->addWidget(m_donationsTable, 1);

    connect(m_cyclesList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) {
            return;
        }
        const int cycleId = m_cyclesList->item(row)->data(Qt::UserRole).toInt();
        m_service->fetchCycleDetail(cycleId);
    });

    return page;
}

void DonanteDetailDialog::markToggled()
{
    m_donante.setActive(m_pendingActive);
    m_toggleBtn->setEnabled(true);
    updateToggleLabel();
}

void DonanteDetailDialog::updateToggleLabel()
{
    m_toggleBtn->setText(m_donante.isActive()
        ? QStringLiteral("Desactivar")
        : QStringLiteral("Activar"));
}

void DonanteDetailDialog::updateLastDonationLabelColor()
{
    if (!m_lastDonationLabel) {
        return;
    }

    const auto cumplido = CycleRuleService::restFulfilled(m_cycleRules, m_donante);
    if (!cumplido.has_value()) {
        m_lastDonationLabel->setStyleSheet(QString());
        return;
    }

    m_lastDonationLabel->setStyleSheet(*cumplido
        ? QStringLiteral("color: #15803D;")
        : QStringLiteral("color: #DC2626;"));
}

void DonanteDetailDialog::refreshAddresses()
{
    m_service->fetchAddresses(m_donante.id());
}

void DonanteDetailDialog::refreshCycles()
{
    m_service->fetchCycles(m_donante.id());
}

void DonanteDetailDialog::onAddresses(const QJsonObject &page)
{
    const QJsonArray items = page.value(QStringLiteral("data")).toArray();

    m_addressesTable->setRowCount(items.size());
    for (int i = 0; i < items.size(); ++i) {
        const QJsonObject obj = items.at(i).toObject();
        const QString id = QString::number(jsonutils::intOr(obj, QStringLiteral("id")));

        m_addressesTable->setItem(i, 0, new QTableWidgetItem(
            jsonutils::stringOr(obj, QStringLiteral("street"))));
        m_addressesTable->setItem(i, 1, new QTableWidgetItem(
            jsonutils::stringOr(obj, QStringLiteral("street_number"))));
        m_addressesTable->setItem(i, 2, new QTableWidgetItem(
            jsonutils::stringOr(obj, QStringLiteral("city"))));
        m_addressesTable->setItem(i, 3, new QTableWidgetItem(
            jsonutils::boolOr(obj, QStringLiteral("is_current"))
                ? QStringLiteral("Sí")
                : QStringLiteral("No")));
        m_addressesTable->setItem(i, 4, new QTableWidgetItem(id));
    }
}

void DonanteDetailDialog::onCycles(const QJsonObject &page)
{
    const QJsonArray items = page.value(QStringLiteral("data")).toArray();

    m_cyclesList->clear();
    for (const QJsonValue &value : items) {
        const QJsonObject obj = value.toObject();
        const int cycleId = jsonutils::intOr(obj, QStringLiteral("id"));
        const QString state = jsonutils::stringOr(obj, QStringLiteral("state"));
        const QString opened = jsonutils::stringOr(obj, QStringLiteral("opened_at"));
        const int donations = jsonutils::intOr(obj, QStringLiteral("donations_count"));

        auto *item = new QListWidgetItem(
            QStringLiteral("Ciclo #%1 · %2 · %3 · %4 donacion%5")
                .arg(cycleId)
                .arg(state == QStringLiteral("open")
                         ? QStringLiteral("Abierto")
                         : QStringLiteral("Cerrado"))
                .arg(opened.left(10))
                .arg(donations)
                .arg(donations == 1 ? QString() : QStringLiteral("es")),
            m_cyclesList);
        item->setData(Qt::UserRole, cycleId);
    }

    if (m_cyclesList->count() > 0) {
        m_cyclesList->setCurrentRow(0);
    } else {
        m_donationsTable->setRowCount(0);
    }
}

void DonanteDetailDialog::onCycleDetail(const QJsonObject &cycle)
{
    const QJsonArray donations = cycle.value(QStringLiteral("donations")).toArray();

    m_donationsTable->setRowCount(donations.size());
    for (int i = 0; i < donations.size(); ++i) {
        const QJsonObject obj = donations.at(i).toObject();

        const QDateTime turn = jsonutils::parseDateTime(
            jsonutils::stringOr(obj, QStringLiteral("appointment_datetime")));
        const QString type = jsonutils::stringOr(obj, QStringLiteral("type_name"));

        auto *numberItem = new QTableWidgetItem(QString::number(i + 1));
        numberItem->setData(Qt::TextAlignmentRole, int(Qt::AlignCenter));

        m_donationsTable->setItem(i, 0, numberItem);
        m_donationsTable->setItem(i, 1, new QTableWidgetItem(
            turn.isValid()
                ? turn.toLocalTime().toString(QStringLiteral("dd/MM/yyyy HH:mm"))
                : QStringLiteral("—")));
        m_donationsTable->setItem(i, 2, new QTableWidgetItem(
            type.isEmpty() ? QStringLiteral("—") : type));
    }
}

void DonanteDetailDialog::onFailed(const ApiError &error)
{
    m_toggleBtn->setEnabled(true);
    if (m_restraintLabel
        && m_restraintLabel->text() == QStringLiteral("Cargando…")) {
        m_restraintLabel->setText(QStringLiteral("No se pudo cargar"));
    }
    Toast::show(
        this,
        error.detail.isEmpty() ? QStringLiteral("Error al comunicarse con el servidor") : error.detail,
        Toast::Type::Error);
}

QWidget *DonanteDetailDialog::buildObservacionesTab()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 12, 8, 8);
    layout->setSpacing(8);

    m_observationsTable = new QTableWidget(0, 2, page);
    m_observationsTable->setHorizontalHeaderLabels({
        QStringLiteral("Fecha"),
        QStringLiteral("Observación"),
    });
    m_observationsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_observationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_observationsTable->verticalHeader()->setVisible(false);
    m_observationsTable->setSelectionMode(QAbstractItemView::NoSelection);

    layout->addWidget(m_observationsTable);

    return page;
}

void DonanteDetailDialog::refreshObservaciones()
{
    m_restraintService->fetchObservations(m_donante.id());
}

void DonanteDetailDialog::refreshRestricciones()
{
    m_restraintService->fetchRestraints(m_donante.id());
}

void DonanteDetailDialog::onObservations(const QJsonArray &items)
{
    m_observationsTable->setRowCount(items.size());
    for (int i = 0; i < items.size(); ++i) {
        const QJsonObject obj = items.at(i).toObject();

        const QString createdAt =
            jsonutils::stringOr(obj, QStringLiteral("created_at"));
        const QString texto =
            jsonutils::stringOr(obj, QStringLiteral("texto"));

        m_observationsTable->setItem(i, 0, new QTableWidgetItem(
            createdAt.left(16)));
        m_observationsTable->setItem(i, 1, new QTableWidgetItem(
            texto.isEmpty() ? QStringLiteral("—") : texto));
    }
}

void DonanteDetailDialog::onRestraints(const QJsonArray &items)
{
    QStringList currentRestrictions;
    const QDate today = QDate::currentDate();
    for (const QJsonValue &value : items) {
        const Restraint restraint = Restraint::fromJson(value.toObject());
        const QDate start = restraint.estimatedStartDate().date();
        const QDate end = restraint.estimatedEndDate().date();

        if (!restraint.isActive()
            || (start.isValid() && start > today)
            || (end.isValid() && end < today)) {
            continue;
        }

        QString description = restraint.motivo();
        if (start.isValid() || end.isValid()) {
            const QString startText = start.isValid()
                ? start.toString(QStringLiteral("dd/MM/yyyy"))
                : QStringLiteral("—");
            const QString endText = end.isValid()
                ? end.toString(QStringLiteral("dd/MM/yyyy"))
                : QStringLiteral("—");
            description += QStringLiteral(" · %1 al %2").arg(startText, endText);
        }
        currentRestrictions.append(description);
    }

    m_restraintLabel->setText(currentRestrictions.isEmpty()
        ? QStringLiteral("Sin restricción")
        : currentRestrictions.join(QStringLiteral("\n")));
}
