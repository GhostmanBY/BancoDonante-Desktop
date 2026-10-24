#include "views/features/usuarios/UsersView.hpp"

#include "models/enums/UserRole.hpp"
#include "services/SessionManager.hpp"
#include "views/components/EmptyState.hpp"
#include "views/components/PaginationBar.hpp"
#include "views/components/Spinner.hpp"
#include "views/components/Toast.hpp"
#include "views/features/usuarios/UserDialog.hpp"
#include "views/features/usuarios/UsersModel.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>
#include <QVBoxLayout>

UsersView::UsersView(QWidget *parent)
    : QWidget(parent),
      m_service(new UsersService(this)),
      m_model(new UsersModel(this))
{
    buildUi();

    connect(m_service, &UsersService::pageFetched, this, [this](const UsersPage &page) {
        m_hasLoadedOnce = true;
        m_model->setItems(page.data);
        m_pagination->setPagination(page.page, page.totalPages, page.total, QStringLiteral("Usuarios"));

        const bool empty = page.data.isEmpty();
        m_bodyStack->setCurrentWidget(empty ? static_cast<QWidget *>(m_empty) : static_cast<QWidget *>(m_table));

        m_pagination->setLoading(false);
        updateEditAction();
    });

    connect(m_service, &UsersService::failed, this, [this](const ApiError &error) {
        m_pagination->setLoading(false);
        if (!m_hasLoadedOnce) {
            m_bodyStack->setCurrentWidget(m_empty);
        }
        const QString message = error.detail.isEmpty()
            ? QStringLiteral("No se pudo cargar la lista de usuarios")
            : error.detail;
        Toast::show(this, message, Toast::Type::Error);
    });

    connect(m_service, &UsersService::createSucceeded, this, [this]() {
        loadPage(m_page);
    });

    connect(m_service, &UsersService::updateSucceeded, this, [this]() {
        loadPage(m_page);
    });

    connect(m_service, &UsersService::activeChanged, this, [this]() {
        Toast::show(this, QStringLiteral("Estado del usuario actualizado"), Toast::Type::Success);
        loadPage(m_page);
    });

    loadPage(1);
}

void UsersView::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("Usuarios"), this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 20px; font-weight: 700; color: #111827; }"));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    m_newBtn = new QPushButton(QStringLiteral("＋  Nuevo"), this);
    m_newBtn->setProperty("variant", "primary");

    m_editBtn = new QPushButton(QStringLiteral("✎  Editar"), this);
    m_editBtn->setEnabled(false);

    m_refreshBtn = new QPushButton(QStringLiteral("Refrescar"), this);

    toolbar->addWidget(m_newBtn);
    toolbar->addWidget(m_editBtn);
    toolbar->addStretch(1);
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
    m_table->setColumnWidth(UsersModel::ColUsuario, 160);
    m_table->setColumnWidth(UsersModel::ColNombre, 200);
    m_table->setColumnWidth(UsersModel::ColMail, 220);
    m_table->setColumnWidth(UsersModel::ColRol, 170);
    m_table->setWordWrap(false);

    m_empty = new EmptyState(
        QStringLiteral("No hay usuarios"),
        QStringLiteral("No se encontraron usuarios para mostrar."),
        QStringLiteral("⌂"),
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

    connect(m_pagination, &PaginationBar::pageChanged, this, &UsersView::onPageChanged);
    connect(m_pagination, &PaginationBar::sizeChanged, this, &UsersView::onSizeChanged);
    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() { loadPage(m_page); });
    connect(m_newBtn, &QPushButton::clicked, this, &UsersView::openCreateDialog);
    connect(m_editBtn, &QPushButton::clicked, this, &UsersView::openEditDialog);
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex &) {
        openEditDialog();
    });

    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QWidget::customContextMenuRequested,
        this, &UsersView::openContextMenu);

    connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, [this](const QModelIndex &, const QModelIndex &) {
            updateEditAction();
        });
}

void UsersView::refresh()
{
    loadPage(m_page);
}

void UsersView::loadPage(int page)
{
    m_page = qMax(1, page);
    m_bodyStack->setCurrentWidget(m_loading);
    m_pagination->setLoading(true);
    m_service->fetchPage(m_page, m_size);
}

void UsersView::onPageChanged(int page)
{
    loadPage(page);
}

void UsersView::onSizeChanged(int size)
{
    m_size = size;
    loadPage(1);
}

const UserItem *UsersView::selectedUser() const
{
    const QModelIndex current = m_table->currentIndex();
    return current.isValid() ? m_model->itemAt(current.row()) : nullptr;
}

// Jerarquía: solo se gestionan usuarios de rango estrictamente inferior
// (admin > area_manager > medical > nurse). Excepción: el admin puede
// editarse a sí mismo (datos/contraseña), pero nunca cambiarse el rol.
bool UsersView::canEdit(const UserItem &item) const
{
    const SessionUser me = SessionManager::instance().user();
    const UserRole theirs = userRoleFromString(item.role, UserRole::NURSE);
    if (me.role == UserRole::ADMIN && item.id == me.id) {
        return true;
    }
    return roleIsAbove(me.role, theirs);
}

// Activar/desactivar nunca aplica al propio usuario (evita bloquearse
// la sesión); solo rangos estrictamente inferiores.
bool UsersView::canToggle(const UserItem &item) const
{
    const SessionUser me = SessionManager::instance().user();
    const UserRole theirs = userRoleFromString(item.role, UserRole::NURSE);
    return roleIsAbove(me.role, theirs);
}

void UsersView::updateEditAction()
{
    const UserItem *selected = selectedUser();
    const bool editable = selected && canEdit(*selected);

    m_editBtn->setEnabled(editable);
    m_editBtn->setToolTip(!selected
        ? QStringLiteral("Seleccioná un usuario")
        : editable
            ? QStringLiteral("Editar el usuario seleccionado")
            : QStringLiteral("Solo se pueden gestionar usuarios de rango inferior"));
}

void UsersView::openEditDialog()
{
    const UserItem *selected = selectedUser();
    if (!selected) {
        return;
    }
    if (!canEdit(*selected)) {
        Toast::show(
            this,
            QStringLiteral("Solo se pueden gestionar usuarios de rango inferior"),
            Toast::Type::Error);
        return;
    }

    auto *dialog = new UserDialog(*selected, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &UserDialog::saved, this, [this]() {
        loadPage(m_page);
    });
    dialog->open();
}

void UsersView::openCreateDialog()
{
    auto *dialog = new UserDialog(std::nullopt, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &UserDialog::saved, this, [this]() {
        loadPage(m_page);
    });
    dialog->open();
}

void UsersView::openContextMenu(const QPoint &pos)
{
    const QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    m_table->selectRow(index.row());

    const UserItem *selected = m_model->itemAt(index.row());
    if (!selected) {
        return;
    }

    QMenu menu(this);

    const bool editable = canEdit(*selected);
    const bool toggleable = canToggle(*selected);

    QAction *edit = menu.addAction(QStringLiteral("✎  Editar…"));
    edit->setEnabled(editable);
    edit->setToolTip(editable
        ? QString()
        : QStringLiteral("Solo se pueden gestionar usuarios de rango inferior"));
    connect(edit, &QAction::triggered, this, &UsersView::openEditDialog);

    menu.addSeparator();

    QAction *toggle = menu.addAction(selected->isActive
        ? QStringLiteral("✗  Desactivar")
        : QStringLiteral("✓  Activar"));
    toggle->setEnabled(toggleable);
    toggle->setToolTip(toggleable
        ? QString()
        : (selected->id == SessionManager::instance().user().id
               ? QStringLiteral("No podés desactivarte a vos mismo")
               : QStringLiteral("Solo se pueden gestionar usuarios de rango inferior")));
    connect(toggle, &QAction::triggered, this,
        [this, active = !selected->isActive]() {
            setActiveForSelected(active);
        });

    menu.exec(m_table->viewport()->mapToGlobal(pos));
}

void UsersView::setActiveForSelected(bool active)
{
    const UserItem *selected = selectedUser();
    if (!selected || selected->isActive == active) {
        return;
    }
    if (!canToggle(*selected)) {
        Toast::show(
            this,
            selected->id == SessionManager::instance().user().id
                ? QStringLiteral("No podés desactivarte a vos mismo")
                : QStringLiteral("Solo se pueden gestionar usuarios de rango inferior"),
            Toast::Type::Error);
        return;
    }

    const QString question = active
        ? QStringLiteral("¿Seguro que querés activar a %1 %2? Podrá iniciar sesión nuevamente.")
              .arg(selected->name, selected->lastname)
        : QStringLiteral("¿Seguro que querés desactivar a %1 %2? No podrá iniciar sesión; "
                         "sus datos se conservan.")
              .arg(selected->name, selected->lastname);

    const auto answer = QMessageBox::question(
        this,
        active ? QStringLiteral("Activar usuario") : QStringLiteral("Desactivar usuario"),
        question,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (answer != QMessageBox::Yes) {
        return;
    }

    m_service->setActive(selected->id, active);
}
