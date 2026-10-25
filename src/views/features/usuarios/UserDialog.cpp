#include "views/features/usuarios/UserDialog.hpp"

#include "models/enums/UserRole.hpp"
#include "services/SessionManager.hpp"
#include "views/components/PasswordReveal.hpp"
#include "views/components/Toast.hpp"

#include <QAction>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

namespace {

struct RoleOption
{
    QString label;
    UserRole value;
};

const RoleOption kRoleOptions[] = {
    {userRoleLabel(UserRole::NURSE), UserRole::NURSE},
    {userRoleLabel(UserRole::MEDICAL), UserRole::MEDICAL},
    {userRoleLabel(UserRole::AREA_MANAGER), UserRole::AREA_MANAGER},
    {userRoleLabel(UserRole::ADMIN), UserRole::ADMIN},
};

} // namespace

UserDialog::UserDialog(const std::optional<UserItem> &user, QWidget *parent)
    : QDialog(parent),
      m_service(new UsersService(this)),
      m_user(user)
{
    setWindowTitle(isCreate() ? QStringLiteral("Nuevo usuario") : QStringLiteral("Editar usuario"));
    setModal(true);
    resize(640, 640);
    setMinimumSize(560, 520);

    buildUi();
    prefill();

    connect(m_service, &UsersService::createSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Usuario creado correctamente"), Toast::Type::Success);
        emit saved();
        accept();
    });
    connect(m_service, &UsersService::updateSucceeded, this, [this]() {
        Toast::show(this, QStringLiteral("Usuario actualizado correctamente"), Toast::Type::Success);
        emit saved();
        accept();
    });
    connect(m_service, &UsersService::failed, this, &UserDialog::onFailed);
}

bool UserDialog::isCreate() const
{
    return !m_user.has_value();
}

void UserDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(14);

    auto *title = new QLabel(
        isCreate()
            ? QStringLiteral("Cargar un nuevo usuario")
            : QStringLiteral("Editar datos del usuario"),
        this);
    title->setStyleSheet(QStringLiteral(
        "QLabel { font-size: 16px; font-weight: 700; color: #111827; }"));

    auto *content = new QWidget();
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 4, 0);
    contentLayout->setSpacing(14);
    contentLayout->addWidget(title);

    auto *form = new QGridLayout();
    form->setSpacing(12);

    m_username = new QLineEdit(this);
    m_username->setPlaceholderText(QStringLiteral("nombre.usuario"));

    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setPlaceholderText(isCreate()
        ? QStringLiteral("Contraseña *")
        : QStringLiteral("Dejar vacío para mantener la actual"));
    m_revealAction = PasswordReveal::attach(m_password);

    m_name = new QLineEdit(this);
    m_lastname = new QLineEdit(this);
    m_mail = new QLineEdit(this);
    m_mail->setPlaceholderText(QStringLiteral("nombre@mail.com"));
    m_phone = new QLineEdit(this);

    m_documentType = new QComboBox(this);
    m_documentType->addItems({
        QStringLiteral("DNI"),
        QStringLiteral("LC"),
        QStringLiteral("LE"),
        QStringLiteral("Pasaporte"),
    });
    m_documentNumber = new QLineEdit(this);

    m_birthDate = new QDateEdit(this);
    m_birthDate->setCalendarPopup(true);
    m_birthDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m_birthDate->setDate(QDate::currentDate().addYears(-30));

    m_birthPlace = new QLineEdit(this);
    m_birthPlace->setPlaceholderText(QStringLiteral("Lugar de nacimiento (opcional)"));

    m_role = new QComboBox(this);
    // Solo se pueden asignar roles de rango estrictamente inferior al del
    // usuario logueado (igual que valida el backend). Si es una
    // auto-edición (solo posible siendo admin) se agrega el rol propio y
    // queda bloqueado: no se puede cambiar el propio rol.
    const SessionUser actor = SessionManager::instance().user();
    for (const RoleOption &option : kRoleOptions) {
        if (!roleIsAbove(actor.role, option.value)) {
            continue;
        }
        m_role->addItem(option.label, static_cast<int>(option.value));
    }

    const bool editingSelf = m_user.has_value() && m_user->id == actor.id;
    if (editingSelf) {
        m_role->addItem(userRoleLabel(actor.role), static_cast<int>(actor.role));
        m_role->setEnabled(false);
        m_role->setToolTip(QStringLiteral("No podés cambiar tu propio rol"));
    }

    auto addRow = [&](int row, int col, const QString &label, QWidget *field) {
        auto *box = new QVBoxLayout();
        box->setSpacing(4);
        auto *caption = new QLabel(label, this);
        caption->setProperty("role", "hint");
        box->addWidget(caption);
        box->addWidget(field);
        form->addLayout(box, row, col);
    };

    addRow(0, 0, QStringLiteral("Usuario *"), m_username);
    addRow(0, 1, isCreate() ? QStringLiteral("Contraseña *") : QStringLiteral("Nueva contraseña (opcional)"), m_password);
    addRow(1, 0, QStringLiteral("Nombre *"), m_name);
    addRow(1, 1, QStringLiteral("Apellido *"), m_lastname);
    addRow(2, 0, QStringLiteral("Mail *"), m_mail);
    addRow(2, 1, QStringLiteral("Teléfono *"), m_phone);
    addRow(3, 0, QStringLiteral("Tipo de documento"), m_documentType);
    addRow(3, 1, QStringLiteral("Nro. de documento *"), m_documentNumber);

    addRow(4, 0, QStringLiteral("Fecha de nacimiento *"), m_birthDate);
    addRow(4, 1, QStringLiteral("Lugar de nacimiento"), m_birthPlace);
    addRow(5, 0, QStringLiteral("Rol"), m_role);

    m_formError = new QLabel(content);
    m_formError->setProperty("role", "error");
    m_formError->setWordWrap(true);
    m_formError->setVisible(false);

    auto *buttons = new QDialogButtonBox(this);
    m_acceptBtn = buttons->addButton(
        isCreate() ? QStringLiteral("Crear usuario") : QStringLiteral("Guardar cambios"),
        QDialogButtonBox::AcceptRole);
    m_acceptBtn->setProperty("variant", "primary");
    buttons->addButton(QStringLiteral("Cancelar"), QDialogButtonBox::RejectRole);

    contentLayout->addLayout(form);
    contentLayout->addWidget(m_formError);
    contentLayout->addStretch();

    auto *scroll = new QScrollArea(this);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setMinimumHeight(440);
    scroll->setMaximumHeight(660);

    layout->addWidget(scroll, 1);
    layout->addWidget(buttons, 0, Qt::AlignRight);

    connect(buttons, &QDialogButtonBox::accepted, this, &UserDialog::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &UserDialog::reject);
}

void UserDialog::prefill()
{
    if (!m_user.has_value()) {
        m_birthDate->setDate(QDate::currentDate().addYears(-30));
        return;
    }

    const UserItem &u = *m_user;
    m_username->setText(u.username);
    m_name->setText(u.name);
    m_lastname->setText(u.lastname);
    m_mail->setText(u.mail);
    m_phone->setText(u.phone);
    m_documentType->setCurrentText(u.documentType);
    m_documentNumber->setText(u.documentNumber);

    const QDate birth = QDate::fromString(u.birthDate, QStringLiteral("yyyy-MM-dd"));
    if (birth.isValid()) {
        m_birthDate->setDate(birth);
    }

    m_birthPlace->setText(u.birthPlace);

    const int roleIndex = m_role->findData(
        static_cast<int>(userRoleFromString(u.role, UserRole::NURSE)));
    if (roleIndex >= 0) {
        m_role->setCurrentIndex(roleIndex);
    }
}

bool UserDialog::validate()
{
    bool ok = true;

    const auto check = [&ok](QLineEdit *field, bool valid) {
        field->setProperty("error", valid ? "false" : "true");
        field->style()->unpolish(field);
        field->style()->polish(field);
        ok = ok && valid;
    };

    check(m_username, !m_username->text().trimmed().isEmpty());
    check(m_name, !m_name->text().trimmed().isEmpty());
    check(m_lastname, !m_lastname->text().trimmed().isEmpty());
    check(m_mail, m_mail->text().contains(QLatin1Char('@')));
    check(m_phone, m_phone->text().trimmed().size() >= 6);
    check(m_documentNumber, !m_documentNumber->text().trimmed().isEmpty());
    if (isCreate()) {
        check(m_password, m_password->text().size() >= 6);
    }

    if (!ok) {
        m_formError->setText(isCreate()
            ? QStringLiteral("Revisá los campos marcados: usuario, contraseña (mín. 6 caracteres), nombre, apellido, mail válido, teléfono (mín. 6 caracteres) y documento.")
            : QStringLiteral("Revisá los campos marcados: usuario, nombre, apellido, mail válido, teléfono (mín. 6 caracteres) y documento."));
        m_formError->setVisible(true);
    } else {
        m_formError->setVisible(false);
    }

    return ok;
}

void UserDialog::submit()
{
    if (!validate()) {
        return;
    }

    m_acceptBtn->setEnabled(false);
    m_revealAction->setEnabled(false);

    UserItem updated = m_user.value_or(UserItem{});
    updated.username = m_username->text().trimmed();
    updated.name = m_name->text().trimmed();
    updated.lastname = m_lastname->text().trimmed();
    updated.mail = m_mail->text().trimmed();
    updated.phone = m_phone->text().trimmed();
    updated.documentType = m_documentType->currentText();
    updated.documentNumber = m_documentNumber->text().trimmed();
    updated.birthPlace = m_birthPlace->text().trimmed();
    updated.role = ::toString(static_cast<UserRole>(m_role->currentData().toInt()));
    updated.birthDate = m_birthDate->date().toString(QStringLiteral("yyyy-MM-dd"));

    if (isCreate()) {
        m_service->create(updated, m_password->text());
    } else {
        m_service->update(m_user->id, updated, m_password->text());
    }
}

void UserDialog::onFailed(const ApiError &error)
{
    m_acceptBtn->setEnabled(true);
    m_revealAction->setEnabled(true);

    QString message = error.detail.isEmpty()
        ? QStringLiteral("No se pudo guardar el usuario.")
        : error.detail;

    if (!error.field.isEmpty()) {
        message = QStringLiteral("%1 (%2)").arg(message, error.field);
    }

    m_formError->setText(message);
    m_formError->setVisible(true);
    Toast::show(this, message, Toast::Type::Error);
}
