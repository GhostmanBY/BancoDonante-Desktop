#include "views/components/TopBar.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace {

QString roleLabel(UserRole role)
{
    switch (role) {
    case UserRole::ADMIN: return QStringLiteral("Administrador");
    case UserRole::AREA_MANAGER: return QStringLiteral("Jefe de área");
    case UserRole::MEDICAL: return QStringLiteral("Personal médico");
    case UserRole::NURSE: return QStringLiteral("Enfermería");
    }
    return {};
}

} // namespace

TopBar::TopBar(QWidget *parent)
    : QFrame(parent)
{
    setFixedHeight(56);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(24, 0, 24, 0);
    layout->setSpacing(12);

    layout->addStretch();

    auto *userBox = new QVBoxLayout();
    userBox->setSpacing(0);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setProperty("role", "user-name");

    m_roleLabel = new QLabel(this);
    m_roleLabel->setProperty("role", "user-role");

    userBox->addWidget(m_nameLabel);
    userBox->addWidget(m_roleLabel);

    m_logoutButton = new QPushButton(QStringLiteral("Cerrar sesión"), this);
    m_logoutButton->setCursor(Qt::PointingHandCursor);

    connect(m_logoutButton, &QPushButton::clicked, this, &TopBar::logoutRequested);

    layout->addLayout(userBox);
    layout->addWidget(m_logoutButton);
}

void TopBar::setUser(const SessionUser &user)
{
    m_nameLabel->setText(user.fullName());
    m_roleLabel->setText(roleLabel(user.role));
}
