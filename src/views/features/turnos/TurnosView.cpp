#include "views/features/turnos/TurnosView.hpp"

#include "models/enums/UserRole.hpp"
#include "services/SessionManager.hpp"
#include "services/WhatsAppService.hpp"
#include "views/components/EmptyState.hpp"
#include "views/components/PaginationBar.hpp"
#include "views/components/Spinner.hpp"
#include "views/components/Toast.hpp"
#include "views/features/turnos/CompleteTurnoDialog.hpp"
#include "views/features/turnos/AssignTurnoDialog.hpp"
#include "views/features/turnos/RescheduleTurnoDialog.hpp"
#include "views/features/turnos/TurnoDetailDialog.hpp"
#include "views/features/turnos/TurnosModel.hpp"

#include <QComboBox>
#include <QDate>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QModelIndex>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>
#include <QVBoxLayout>

namespace {

struct EstadoOption
{
    QString label;
    std::optional<AppointmentState> state;
};

const EstadoOption kEstadoOptions[] = {
    {QStringLiteral("Todos los estados"), std::nullopt},
    {appointmentStateLabel(AppointmentState::PENDING), AppointmentState::PENDING},
    {appointmentStateLabel(AppointmentState::CONFIRMED), AppointmentState::CONFIRMED},
    {appointmentStateLabel(AppointmentState::COMPLETED), AppointmentState::COMPLETED},
    {appointmentStateLabel(AppointmentState::CANCELLED), AppointmentState::CANCELLED},
    {appointmentStateLabel(AppointmentState::RESCHEDULED), AppointmentState::RESCHEDULED},
    {appointmentStateLabel(AppointmentState::NO_SHOW), AppointmentState::NO_SHOW},
};

} // namespace

TurnosView::TurnosView(QWidget *parent)
    : QWidget(parent),
      m_service(new AppointmentService(this)),
      m_whatsAppService(new WhatsAppService(this)),
      m_model(new TurnosModel(this))
{
    buildUi();

    connect(m_service, &AppointmentService::pageFetched, this, [this](const TurnosPage &page) {
        m_hasLoadedOnce = true;
        m_model->setItems(page.data);
        m_pagination->setPagination(page.page, page.totalPages, page.total, QStringLiteral("Turnos"));

        const bool empty = page.data.isEmpty();
        m_bodyStack->setCurrentWidget(empty ? static_cast<QWidget *>(m_empty) : static_cast<QWidget *>(m_table));

        m_pagination->setLoading(false);

        markExpiredAsNoShow(page.data);
    });

    connect(m_service, &AppointmentService::failed, this, [this](const ApiError &error) {
        m_pagination->setLoading(false);
        if (!m_hasLoadedOnce) {
            m_bodyStack->setCurrentWidget(m_empty);
        }
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudo cargar la lista de turnos")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
    });

    connect(m_service, &AppointmentService::changeStateSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Estado del turno actualizado"), Toast::Type::Success);
        loadPage(m_page);
    });

    connect(m_service, &AppointmentService::expiredNoShowDone, this, [this]() {
        loadPage(m_page);
    });

    connect(m_whatsAppService, &WhatsAppService::templatesFetched, this,
        [this](const QList<WSPTemplateOption> &templates) {
            const QModelIndex current = m_table->currentIndex();
            const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
            if (!selected || selected->state != AppointmentState::PENDING || selected->patientId <= 0) {
                updateNotifyButton();
                return;
            }
            if (templates.isEmpty()) {
                Toast::show(this, QStringLiteral("No hay plantillas activas para notificar"), Toast::Type::Error);
                return;
            }

            QStringList labels;
            for (const WSPTemplateOption &option : templates) {
                labels.append(option.name);
            }
            bool accepted = false;
            const QString chosen = QInputDialog::getItem(
                this, QStringLiteral("Notificar por WhatsApp"),
                QStringLiteral("Elegí el motivo de la notificación:"), labels, 0, false, &accepted);
            if (!accepted) {
                return;
            }
            const int index = labels.indexOf(chosen);
            if (index >= 0 && index < templates.size()) {
                m_notifyBtn->setEnabled(false);
                m_whatsAppService->enqueue(selected->patientId, templates.at(index).motive);
            }
        });

    connect(m_whatsAppService, &WhatsAppService::enqueueSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Notificación agregada a la cola de WhatsApp"), Toast::Type::Success);
        updateNotifyButton();
    });

    connect(m_whatsAppService, &WhatsAppService::failed, this, [this](const ApiError &error) {
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudo completar la operación de WhatsApp")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
        updateNotifyButton();
    });

    loadPage(1);
}

void TurnosView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("Turnos"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 20px; font-weight: 700; color: #111827; }"));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    m_stateCombo = new QComboBox(this);
    for (const EstadoOption &option : kEstadoOptions) {
        m_stateCombo->addItem(option.label);
    }

    m_refreshBtn = new QPushButton(QStringLiteral("Refrescar"), this);
    m_notifyBtn = new QPushButton(QStringLiteral("Notificar por WhatsApp"), this);
    m_notifyBtn->setEnabled(false);

    toolbar->addWidget(m_stateCombo);
    toolbar->addStretch(1);
    toolbar->addWidget(m_notifyBtn);
    toolbar->addWidget(m_refreshBtn);

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
    m_table->setColumnWidth(TurnosModel::ColFecha, 140);
    m_table->setColumnWidth(TurnosModel::ColPaciente, 200);
    m_table->setColumnWidth(TurnosModel::ColTipo, 150);
    m_table->setColumnWidth(TurnosModel::ColEstado, 120);
    m_table->setColumnWidth(TurnosModel::ColResponsable, 190);
    m_table->setWordWrap(false);

    m_empty = new EmptyState(
        QStringLiteral("No hay turnos"),
        QStringLiteral("Probá ajustar el filtro de estado."),
        QStringLiteral("◷"),
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

    connect(m_stateCombo, &QComboBox::currentIndexChanged, this, &TurnosView::onStateFilterChanged);
    connect(m_pagination, &PaginationBar::pageChanged, this, &TurnosView::onPageChanged);
    connect(m_pagination, &PaginationBar::sizeChanged, this, &TurnosView::onSizeChanged);
    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() { loadPage(m_page); });
    connect(m_notifyBtn, &QPushButton::clicked, this, &TurnosView::notifySelectedPatient);
    connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, [this]() { updateNotifyButton(); });
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if (index.isValid()) {
            m_table->selectRow(index.row());
            openDetailDialog();
        }
    });

    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QWidget::customContextMenuRequested,
        this, &TurnosView::openContextMenu);
}

void TurnosView::updateNotifyButton()
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    m_notifyBtn->setEnabled(selected
        && selected->state == AppointmentState::PENDING
        && selected->patientId > 0);
}

void TurnosView::notifySelectedPatient()
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected || selected->state != AppointmentState::PENDING || selected->patientId <= 0) {
        updateNotifyButton();
        return;
    }

    m_notifyBtn->setEnabled(false);
    m_whatsAppService->fetchActiveTemplates();
}

void TurnosView::refresh()
{
    loadPage(m_page);
}

void TurnosView::loadPage(int page)
{
    m_page = qMax(1, page);
    m_bodyStack->setCurrentWidget(m_loading);
    m_pagination->setLoading(true);
    m_service->fetchPage(m_page, m_size, selectedStateFilter());
}

void TurnosView::onStateFilterChanged()
{
    loadPage(1);
}

void TurnosView::onPageChanged(int page)
{
    loadPage(page);
}

void TurnosView::onSizeChanged(int size)
{
    m_size = size;
    loadPage(1);
}

std::optional<AppointmentState> TurnosView::selectedStateFilter() const
{
    const int index = m_stateCombo->currentIndex();
    if (index <= 0) {
        return std::nullopt;
    }
    return kEstadoOptions[index].state;
}

bool TurnosView::canWrite() const
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

void TurnosView::openContextMenu(const QPoint &pos)
{
    const QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    m_table->selectRow(index.row());

    const TurnoItem *selected = m_model->itemAt(index.row());
    if (!selected) {
        return;
    }

    struct StateAction
    {
        QString label;
        AppointmentState state;
    };

    const StateAction actions[] = {
        {QStringLiteral("Confirmar"), AppointmentState::CONFIRMED},
        {QStringLiteral("Marcar ausente"), AppointmentState::NO_SHOW},
        {QStringLiteral("Cancelar"), AppointmentState::CANCELLED},
    };

    const bool write = canWrite();
    const UserRole role = SessionManager::instance().user().role;
    const bool canAssign = role == UserRole::ADMIN || role == UserRole::AREA_MANAGER
        || role == UserRole::MEDICAL || role == UserRole::NURSE;

    QMenu menu(this);

    QAction *completar = menu.addAction(QStringLiteral("✎  Completar…"));
    const bool canComplete = write
        && selected->state != AppointmentState::COMPLETED
        && selected->datetime.date() == QDate::currentDate();
    completar->setEnabled(canComplete);
    if (selected->assignedUsers.trimmed().isEmpty()) {
        completar->setEnabled(false);
        completar->setToolTip(QStringLiteral(
            "Asigná un responsable al turno antes de completarlo."));
    }

    QAction *reprogramar = menu.addAction(QStringLiteral("↻  Reprogramar…"));
    const bool canReschedule = write
        && selected->state != AppointmentState::COMPLETED;
    reprogramar->setEnabled(canReschedule);

    QAction *asignar = menu.addAction(
        role == UserRole::MEDICAL || role == UserRole::NURSE
            ? QStringLiteral("Asignarme este turno")
            : QStringLiteral("Asignar a médico o enfermero…"));
    const bool assignmentBlockedState = selected->state == AppointmentState::CANCELLED
        || selected->state == AppointmentState::NO_SHOW;
    asignar->setEnabled(canAssign && !assignmentBlockedState);

    menu.addSeparator();

    QList<QAction *> stateActions;
    stateActions.reserve(static_cast<int>(std::size(actions)));
    for (const StateAction &action : actions) {
        QAction *qAction = menu.addAction(action.label);
        qAction->setEnabled(write
            && selected->state != AppointmentState::COMPLETED
            && action.state != selected->state);
        stateActions.append(qAction);
    }

    QAction *chosen = menu.exec(m_table->viewport()->mapToGlobal(pos));
    if (!chosen) {
        return;
    }

    if (chosen == completar) {
        if (canComplete) {
            openCompleteDialog();
        }
        return;
    }

    if (chosen == reprogramar) {
        if (canReschedule) {
            openRescheduleDialog();
        }
        return;
    }

    if (chosen == asignar) {
        auto *dialog = new AssignTurnoDialog(selected->id, this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, &AssignTurnoDialog::assigned, this, [this]() {
            loadPage(m_page);
        });
        dialog->open();
        return;
    }

    const int row = stateActions.indexOf(chosen);
    if (row >= 0) {
        changeSelectedState(actions[row].state);
    }
}

void TurnosView::openCompleteDialog()
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected || !canWrite()) {
        return;
    }
    if (selected->assignedUsers.trimmed().isEmpty()) {
        Toast::show(
            this,
            QStringLiteral("No se puede completar el turno: asigná un responsable primero."),
            Toast::Type::Error);
        return;
    }

    auto *dialog = new CompleteTurnoDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &CompleteTurnoDialog::completed, this, [this]() {
        loadPage(m_page);
    });
    dialog->open();
}

void TurnosView::openDetailDialog()
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected) {
        return;
    }

    auto *dialog = new TurnoDetailDialog(selected->id, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->open();
}

void TurnosView::openRescheduleDialog()
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected || !canWrite() || selected->state == AppointmentState::COMPLETED) {
        return;
    }

    auto *dialog = new RescheduleTurnoDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &RescheduleTurnoDialog::rescheduled, this, [this]() {
        loadPage(m_page);
    });
    dialog->open();
}

void TurnosView::markExpiredAsNoShow(const QList<TurnoItem> &items)
{
    if (!canWrite()) {
        return;
    }

    const QDate today = QDate::currentDate();
    QList<int> expired;
    for (const TurnoItem &item : items) {
        const bool autoMarkable = item.state == AppointmentState::PENDING
            || item.state == AppointmentState::RESCHEDULED;
        if (autoMarkable
            && item.datetime.isValid()
            && item.datetime.date() < today
            && !m_autoNoShowTried.contains(item.id)) {
            m_autoNoShowTried.insert(item.id);
            expired.append(item.id);
        }
    }

    if (!expired.isEmpty()) {
        m_service->autoNoShowExpired(expired);
    }
}

void TurnosView::changeSelectedState(AppointmentState state)
{
    const QModelIndex current = m_table->currentIndex();
    const TurnoItem *selected = current.isValid() ? m_model->itemAt(current.row()) : nullptr;
    if (!selected || !canWrite() || selected->state == state) {
        return;
    }

    m_service->changeState(selected->id, state);
}
