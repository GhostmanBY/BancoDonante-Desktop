#include "views/features/usuarios/UsersModel.hpp"

#include "models/enums/UserRole.hpp"

#include <QFont>

namespace {

QColor blendOnWhite(const QColor &color, double strength)
{
    const double r = 255.0 + (color.red() - 255.0) * strength;
    const double g = 255.0 + (color.green() - 255.0) * strength;
    const double b = 255.0 + (color.blue() - 255.0) * strength;
    return QColor(qRound(r), qRound(g), qRound(b));
}

} // namespace

UsersModel::UsersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int UsersModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

int UsersModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QColor UsersModel::tint(const QColor &color, double strength)
{
    return blendOnWhite(color, strength);
}

QColor UsersModel::rolColor(const QString &role)
{
    const UserRole value = userRoleFromString(role, UserRole::NURSE);
    switch (value) {
    case UserRole::ADMIN: return QColor(QStringLiteral("#991B1B"));
    case UserRole::MEDICAL: return QColor(QStringLiteral("#1D4ED8"));
    case UserRole::AREA_MANAGER: return QColor(QStringLiteral("#92400E"));
    case UserRole::NURSE: return QColor(QStringLiteral("#065F46"));
    }
    return {};
}

void UsersModel::setItems(const QList<UserItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

const UserItem *UsersModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return nullptr;
    }
    return &m_items.at(row);
}

QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) {
        return {};
    }

    const UserItem &item = m_items.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColUsuario: return item.username;
        case ColNombre: return item.name + QLatin1Char(' ') + item.lastname;
        case ColMail: return item.mail;
        case ColRol: return userRoleLabel(userRoleFromString(item.role, UserRole::NURSE));
        case ColActivo: return item.isActive ? QStringLiteral("Activo") : QStringLiteral("Inactivo");
        }
        return {};
    }

    if (role == Qt::FontRole && index.column() == ColUsuario) {
        QFont font;
        font.setBold(true);
        return font;
    }

    if (role == Qt::ForegroundRole) {
        if (index.column() == ColRol) {
            return rolColor(item.role);
        }
        if (index.column() == ColActivo) {
            return item.isActive
                ? QColor(QStringLiteral("#065F46"))
                : QColor(QStringLiteral("#6B7280"));
        }
        if (!item.isActive) {
            return QColor(QStringLiteral("#6B7280"));
        }
        return {};
    }

    if (role == Qt::BackgroundRole && index.column() == ColActivo) {
        const QColor base = item.isActive
            ? QColor(QStringLiteral("#065F46"))
            : QColor(QStringLiteral("#6B7280"));
        return tint(base, 0.08);
    }

    return {};
}

QVariant UsersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case ColUsuario: return QStringLiteral("Usuario");
    case ColNombre: return QStringLiteral("Nombre");
    case ColMail: return QStringLiteral("Mail");
    case ColRol: return QStringLiteral("Rol");
    case ColActivo: return QStringLiteral("Activo");
    }
    return {};
}
