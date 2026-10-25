#include "views/features/turnos/AssignTurnoDialog.hpp"

#include "models/enums/UserRole.hpp"
#include "services/SessionManager.hpp"
#include "services/UserXAppointmentService.hpp"
#include "views/components/Toast.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

AssignTurnoDialog::AssignTurnoDialog(int appointmentId, QWidget *parent)
    : QDialog(parent),
      m_appointmentId(appointmentId),
      m_usersService(new UsersService(this)),
      m_assignmentService(new UserXAppointmentService(this))
{
    const SessionUser actor = SessionManager::instance().user();
    m_canChooseUser = actor.role == UserRole::ADMIN || actor.role == UserRole::AREA_MANAGER;
    setWindowTitle(QStringLiteral("Asignar turno"));
    setModal(true);
    setMinimumWidth(380);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;
    m_user = new QComboBox(this);
    m_user->setObjectName(QStringLiteral("appointmentAssignee"));
    form->addRow(QStringLiteral("Asignar a"), m_user);
    layout->addLayout(form);

    m_error = new QLabel(this);
    m_error->setProperty("role", "error");
    m_error->setWordWrap(true);
    m_error->hide();
    layout->addWidget(m_error);

    auto *buttons = new QDialogButtonBox(this);
    m_accept = buttons->addButton(QStringLiteral("Asignar"), QDialogButtonBox::AcceptRole);
    m_accept->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);
    layout->addWidget(buttons, 0, Qt::AlignRight);
    connect(buttons, &QDialogButtonBox::accepted, this, &AssignTurnoDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &AssignTurnoDialog::reject);

    connect(m_usersService, &UsersService::pageFetched, this, &AssignTurnoDialog::onUsersFetched);
    connect(m_usersService, &UsersService::failed, this, &AssignTurnoDialog::onFailed);
    connect(m_assignmentService, &UserXAppointmentService::createSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Turno asignado correctamente"), Toast::Type::Success);
        emit assigned();
        accept();
    });
    connect(m_assignmentService, &UserXAppointmentService::failed, this, &AssignTurnoDialog::onFailed);

    if (m_canChooseUser) {
        m_user->addItem(QStringLiteral("Cargando usuarios…"), -1);
        m_user->setEnabled(false);
        m_accept->setEnabled(false);
        m_usersService->fetchPage(1, 100);
    } else {
        m_user->addItem(actor.fullName(), actor.id);
    }
}

void AssignTurnoDialog::onUsersFetched(const UsersPage &page)
{
    m_user->clear();
    for (const UserItem &user : page.data) {
        const UserRole role = userRoleFromString(user.role, UserRole::NURSE);
        if (user.isActive && (role == UserRole::MEDICAL || role == UserRole::NURSE)) {
            m_user->addItem(QStringLiteral("%1 %2 · %3")
                .arg(user.name, user.lastname, userRoleLabel(role)), user.id);
        }
    }
    if (m_user->count() == 0) {
        m_user->addItem(QStringLiteral("No hay médicos o enfermeros activos"), -1);
    }
    m_user->setEnabled(m_user->count() > 0 && m_user->currentData().toInt() >= 0);
    m_accept->setEnabled(m_user->isEnabled());
}

void AssignTurnoDialog::onFailed(const ApiError &error)
{
    const QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo completar la operación") : error.detail;
    m_error->setText(message);
    m_error->show();
    if (m_canChooseUser) m_accept->setEnabled(m_user->currentData().toInt() >= 0);
    else m_accept->setEnabled(true);
    Toast::show(this, message, Toast::Type::Error);
}

void AssignTurnoDialog::submit()
{
    const int userId = m_user->currentData().toInt();
    if (userId <= 0) return;
    m_error->hide();
    m_accept->setEnabled(false);
    m_assignmentService->create(userId, m_appointmentId);
}
