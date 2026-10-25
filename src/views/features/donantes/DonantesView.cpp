#include "views/features/donantes/DonantesView.hpp"

#include "models/enums/UserRole.hpp"
#include "services/DonorService.hpp"
#include "services/SessionManager.hpp"
#include "views/components/EmptyState.hpp"
#include "views/components/PaginationBar.hpp"
#include "views/components/Spinner.hpp"
#include "views/components/Toast.hpp"
#include "views/features/donantes/AppointmentDialog.hpp"
#include "views/features/donantes/DonanteDetailDialog.hpp"
#include "views/features/donantes/DonanteDialog.hpp"
#include "views/features/donantes/DonantesModel.hpp"
#include "views/features/donantes/RestraintCatalogDialog.hpp"

#include <QComboBox>
#include <QDate>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLocale>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

namespace {

struct EstadoOption
{
    QString label;
    std::optional<PatientState> state;
};

const EstadoOption kEstadoOptions[] = {
    {QStringLiteral("Todos los estados"), std::nullopt},
    {QStringLiteral("Apto"), PatientState::APTO},
    {QStringLiteral("En revisión"), PatientState::REVISON},
    {QStringLiteral("No apto"), PatientState::NO_APTO},
    {QStringLiteral("Suspendido"), PatientState::SUSPENDED},
};

QString estadoLabel(PatientState state)
{
    for (const auto &option : kEstadoOptions) {
        if (option.state == state) {
            return option.label;
        }
    }

    return {};
}

} // namespace

DonantesView::DonantesView(QWidget *parent)
    : QWidget(parent),
      m_service(new DonorService(this)),
      m_model(new DonantesModel(this)),
      m_ruleService(new CycleRuleService(this))
{
    buildUi();

    connect(m_ruleService, &CycleRuleService::fetched, this,
        [this](const QList<CycleRuleItem> &rules) {
            m_cycleRules = rules;
            m_rulesLoaded = true;
            m_model->setCycleRules(rules);

            if (m_pendingAppointment.has_value()) {
                const Patientes donor = *m_pendingAppointment;
                m_pendingAppointment.reset();
                openAppointmentFor(donor);
            }
        });

    connect(m_ruleService, &CycleRuleService::failed, this, [this](const ApiError &) {
        if (m_pendingAppointment.has_value()) {
            const Patientes donor = *m_pendingAppointment;
            m_pendingAppointment.reset();

            if (m_rulesLoaded) {
                openAppointmentFor(donor);
                return;
            }

            const QString message = QStringLiteral(
                "No pudimos verificar el historial de donaciones de %1. "
                "¿Querés continuar con la asignación del turno?")
                .arg(QStringLiteral("%1 %2").arg(donor.name(), donor.lastname()));
            if (confirmAppointment(message)) {
                openAppointmentFor(donor);
            }
            return;
        }

        if (!m_rulesLoaded) {
            Toast::show(
                this,
                QStringLiteral("No se pudieron cargar las reglas de descanso"),
                Toast::Type::Error);
        }
    });

    connect(m_service, &DonorService::pageFetched, this, [this](const DonantesPage &page) {
        m_hasLoadedOnce = true;
        m_model->setItems(page.data);
        m_pagination->setPagination(page.page, page.totalPages, page.total, QStringLiteral("Donantes"));

        const bool empty = page.data.isEmpty();
        m_bodyStack->setCurrentWidget(empty ? static_cast<QWidget *>(m_empty) : static_cast<QWidget *>(m_table));

        m_pagination->setLoading(false);
        updateActionButtons();
    });

    connect(m_service, &DonorService::failed, this, [this](const ApiError &error) {
        m_pagination->setLoading(false);
        if (!m_hasLoadedOnce) {
            m_bodyStack->setCurrentWidget(m_empty);
        }
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudo cargar la lista de donantes")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
    });

    connect(m_service, &DonorService::createSucceeded, this, [this]() {
        loadPage(m_page);
    });
    connect(m_service, &DonorService::updateSucceeded, this, [this]() {
        loadPage(m_page);
    });
    connect(m_service, &DonorService::toggleSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Estado del donante actualizado"), Toast::Type::Success);
        loadPage(m_page);
    });

    loadPage(1);
    m_ruleService->fetchAll();
}

void DonantesView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("Donantes"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 20px; font-weight: 700; color: #111827; }"));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(QStringLiteral("Buscar por nombre, apellido, documento, mail o teléfono…"));
    m_search->setClearButtonEnabled(true);

    m_stateCombo = new QComboBox(this);
    for (const EstadoOption &option : kEstadoOptions) {
        m_stateCombo->addItem(option.label);
    }

    m_activeCombo = new QComboBox(this);
    m_activeCombo->addItems({
        QStringLiteral("Todos"),
        QStringLiteral("Activos"),
        QStringLiteral("Inactivos"),
    });

    m_createBtn = new QPushButton(QStringLiteral("+ Nuevo donante"), this);
    m_createBtn->setProperty("variant", "primary");

    m_editBtn = new QPushButton(QStringLiteral("Editar"), this);
    m_toggleBtn = new QPushButton(this);
    m_refreshBtn = new QPushButton(QStringLiteral("Refrescar"), this);

    toolbar->addWidget(m_search, 1);
    toolbar->addWidget(m_stateCombo);
    toolbar->addWidget(m_activeCombo);
    toolbar->addSpacing(8);
    toolbar->addWidget(m_refreshBtn);
    toolbar->addWidget(m_editBtn);
    toolbar->addWidget(m_toggleBtn);
    toolbar->addWidget(m_createBtn);

    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->setColumnWidth(DonantesModel::ColDonante, 180);
    m_table->setColumnWidth(DonantesModel::ColDocumento, 130);
    m_table->setColumnWidth(DonantesModel::ColGrupo, 70);
    m_table->setColumnWidth(DonantesModel::ColEstado, 110);
    m_table->setColumnWidth(DonantesModel::ColUltimaDonacion, 180);
    m_table->setColumnWidth(DonantesModel::ColMail, 220);
    m_table->setColumnWidth(DonantesModel::ColTelefono, 130);
    m_table->setColumnWidth(DonantesModel::ColActivo, 70);
    m_table->setWordWrap(false);

    m_empty = new EmptyState(
        QStringLiteral("No hay donantes"),
        QStringLiteral("Probá ajustar la búsqueda o los filtros."),
        QStringLiteral("♥"),
        this);

    m_loading = new QWidget(this);
    auto *loadingLayout = new QVBoxLayout(m_loading);
    auto *spinner = new Spinner(m_loading);
    spinner->start();
    loadingLayout->addStretch();
    loadingLayout->addWidget(spinner, 0, Qt::AlignCenter);
    loadingLayout->addStretch();

    m_bodyStack = new QStackedWidget(this);
    m_bodyStack->addWidget(m_table);
    m_bodyStack->addWidget(m_empty);
    m_bodyStack->addWidget(m_loading);

    m_pagination = new PaginationBar(this);

    layout->addWidget(title);
    layout->addLayout(toolbar);
    layout->addWidget(m_bodyStack, 1);
    layout->addWidget(m_pagination);

    m_debounce = new QTimer(this);
    m_debounce->setSingleShot(true);
    m_debounce->setInterval(350);

    connect(m_search, &QLineEdit::textChanged, this, &DonantesView::onSearchChanged);
    connect(m_debounce, &QTimer::timeout, this, &DonantesView::onSearchDebounced);
    connect(m_stateCombo, &QComboBox::currentIndexChanged, this, &DonantesView::onStateFilterChanged);
    connect(m_activeCombo, &QComboBox::currentIndexChanged, this, &DonantesView::onActiveFilterChanged);
    connect(m_pagination, &PaginationBar::pageChanged, this, &DonantesView::onPageChanged);
    connect(m_pagination, &PaginationBar::sizeChanged, this, &DonantesView::onSizeChanged);
    connect(m_createBtn, &QPushButton::clicked, this, &DonantesView::openCreateDialog);
    connect(m_editBtn, &QPushButton::clicked, this, &DonantesView::openEditDialog);
    connect(m_toggleBtn, &QPushButton::clicked, this, &DonantesView::toggleSelected);
    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() { loadPage(m_page); });
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex &) {
        openDetail();
    });
    connect(m_table->selectionModel(), &QItemSelectionModel::currentChanged,
        this, [this]() { updateActionButtons(); });

    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QWidget::customContextMenuRequested,
        this, &DonantesView::openContextMenu);

    updateActionButtons();
}

void DonantesView::refresh()
{
    m_ruleService->fetchAll();
    loadPage(m_page);
}

void DonantesView::loadPage(int page)
{
    m_page = qMax(1, page);
    m_bodyStack->setCurrentWidget(m_loading);
    m_pagination->setLoading(true);
    m_service->fetchPage(
        m_page,
        m_size,
        m_search->text().trimmed(),
        selectedStateFilter(),
        selectedActiveFilter());
}

void DonantesView::onSearchChanged()
{
    m_debounce->start();
}

void DonantesView::onSearchDebounced()
{
    loadPage(1);
}

void DonantesView::onStateFilterChanged()
{
    loadPage(1);
}

void DonantesView::onActiveFilterChanged()
{
    loadPage(1);
}

void DonantesView::onPageChanged(int page)
{
    loadPage(page);
}

void DonantesView::onSizeChanged(int size)
{
    m_size = size;
    loadPage(1);
}

std::optional<PatientState> DonantesView::selectedStateFilter() const
{
    const int index = m_stateCombo->currentIndex();
    if (index <= 0) {
        return std::nullopt;
    }
    return kEstadoOptions[index].state;
}

std::optional<bool> DonantesView::selectedActiveFilter() const
{
    switch (m_activeCombo->currentIndex()) {
    case 1: return true;
    case 2: return false;
    default: return std::nullopt;
    }
}

bool DonantesView::canWrite() const
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

bool DonantesView::canToggle() const
{
    switch (SessionManager::instance().user().role) {
    case UserRole::ADMIN:
    case UserRole::AREA_MANAGER:
        return true;
    case UserRole::MEDICAL:
    case UserRole::NURSE:
        return false;
    }
    return false;
}

void DonantesView::updateActionButtons()
{
    const bool write = canWrite();
    const bool toggle = canToggle();

    m_createBtn->setEnabled(write);
    m_editBtn->setEnabled(write);
    m_toggleBtn->setEnabled(toggle);

    m_createBtn->setVisible(write);
    m_editBtn->setVisible(write);
    m_toggleBtn->setVisible(toggle);

    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    const bool hasSelection = selected != nullptr;

    m_editBtn->setEnabled(write && hasSelection);
    m_toggleBtn->setEnabled(toggle && hasSelection);

    if (hasSelection) {
        m_toggleBtn->setText(selected->isActive()
            ? QStringLiteral("Desactivar")
            : QStringLiteral("Activar"));
    } else {
        m_toggleBtn->setText(QStringLiteral("Activar / Desactivar"));
    }
}

void DonantesView::openCreateDialog()
{
    auto *dialog = new DonanteDialog(std::nullopt, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &DonanteDialog::saved, this, [this]() {
        loadPage(1);
    });
    dialog->open();
}

void DonantesView::openEditDialog()
{
    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    auto *dialog = new DonanteDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &DonanteDialog::saved, this, [this]() {
        loadPage(m_page);
    });
    dialog->open();
}

void DonantesView::toggleSelected()
{
    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    if (selected->isActive()) {
        m_service->deactivate(selected->id());
    } else {
        m_service->activate(selected->id());
    }
}

void DonantesView::openContextMenu(const QPoint &pos)
{
    const QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    m_table->selectRow(index.row());

    QMenu menu(this);
    QAction *darTurno = menu.addAction(QStringLiteral("♥  Dar turno"));
    darTurno->setEnabled(canWrite());
    QAction *asignarRestriccion = menu.addAction(QStringLiteral("⚠  Restricciones…"));
    asignarRestriccion->setEnabled(canWrite());
    QAction *verDetalle = menu.addAction(QStringLiteral("Ver detalle"));

    QAction *chosen = menu.exec(m_table->viewport()->mapToGlobal(pos));
    if (chosen == darTurno) {
        openAppointmentDialog();
    } else if (chosen == asignarRestriccion) {
        openRestraintCatalog();
    } else if (chosen == verDetalle) {
        openDetail();
    }
}

void DonantesView::openAppointmentDialog()
{
    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    if (!m_rulesLoaded) {
        m_pendingAppointment = *selected;
        m_ruleService->fetchAll();
        return;
    }

    openAppointmentFor(*selected);
}

void DonantesView::openAppointmentFor(const Patientes &donor)
{
    if (donor.stateDonation() != PatientState::APTO) {
        const QString nombre =
            QStringLiteral("%1 %2").arg(donor.name(), donor.lastname());

        Toast::show(
            this,
            QStringLiteral("%1 no puede donar: estado actual '%2'. "
                           "Se necesita una evaluación médica previa.")
                .arg(nombre, estadoLabel(donor.stateDonation())),
            Toast::Type::Error);
        return;
    }

    const auto rule = CycleRuleService::ruleFor(m_cycleRules, donor);

    if (rule.has_value()
        && donor.lastDonationDate().has_value()
        && donor.lastDonationDate()->isValid()) {
        const int wait = CycleRuleService::waitDaysFor(*rule, donor.hasOpenCycle());
        const QDate disponible = donor.lastDonationDate()->date().addDays(wait);

        if (QDate::currentDate() < disponible) {
            const QLocale esAr(QLocale::Spanish, QLocale::Argentina);
            const QString nombre =
                QStringLiteral("%1 %2").arg(donor.name(), donor.lastname());
            const QString desde = esAr.toString(
                disponible, QStringLiteral("d 'de' MMMM 'de' yyyy"));
            const QString serie = donor.hasOpenCycle()
                ? QStringLiteral("continuar la serie actual")
                : QStringLiteral("iniciar una serie nueva");

            const QString message = QStringLiteral(
                "La última donación registrada de %1 fue de tipo %2. "
                "Según el plazo configurado, debería esperar %3 días (%4); "
                "podría volver a donar desde el %5.\n\n"
                "¿Querés continuar y asignar el turno de todos modos?")
                .arg(nombre)
                .arg(rule->typeDonationName)
                .arg(wait)
                .arg(serie)
                .arg(desde);

            if (!confirmAppointment(message)) {
                return;
            }
        }
    }

    auto *dialog = new AppointmentDialog(donor, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->open();
}

bool DonantesView::confirmAppointment(const QString &message)
{
    QMessageBox box(
        QMessageBox::Warning,
        QStringLiteral("Revisá el plazo entre donaciones"),
        message,
        QMessageBox::NoButton,
        this);

    QPushButton *cancelar = box.addButton(QStringLiteral("Cancelar"), QMessageBox::RejectRole);
    box.addButton(QStringLiteral("Continuar"), QMessageBox::AcceptRole);
    box.setDefaultButton(cancelar);
    box.exec();

    return box.clickedButton() != cancelar;
}

void DonantesView::openRestraintCatalog()
{
    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    auto *dialog = new RestraintCatalogDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RestraintCatalogDialog::assigned, this, [this](int) {
        loadPage(m_page);
    });
    dialog->open();
}

void DonantesView::openDetail()
{
    const QModelIndex current = m_table->currentIndex();
    const Patientes *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    auto *dialog = new DonanteDetailDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog, &DonanteDetailDialog::toggleRequested, this,
        [this, dialog](int donanteId, bool activate) {
            dialog->setPendingActive(activate);
            if (activate) {
                m_service->activate(donanteId);
            } else {
                m_service->deactivate(donanteId);
            }
        });
    connect(m_service, &DonorService::toggleSucceeded, dialog, [dialog]() {
        dialog->markToggled();
    });

    dialog->open();
}
