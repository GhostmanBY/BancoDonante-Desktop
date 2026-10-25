#include "views/features/turnos/TurnosModel.hpp"

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

TurnosModel::TurnosModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TurnosModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

int TurnosModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QColor TurnosModel::tint(const QColor &color, double strength)
{
    return blendOnWhite(color, strength);
}

QString TurnosModel::tipoLabel(AppointmentType type)
{
    switch (type) {
    case AppointmentType::DONATION: return QStringLiteral("Turno de donación");
    case AppointmentType::CHECK: return QStringLiteral("Control médico");
    }
    return {};
}

QColor TurnosModel::estadoColor(AppointmentState state)
{
    switch (state) {
    case AppointmentState::PENDING: return QColor(QStringLiteral("#92400E"));
    case AppointmentState::CONFIRMED: return QColor(QStringLiteral("#1E40AF"));
    case AppointmentState::COMPLETED: return QColor(QStringLiteral("#065F46"));
    case AppointmentState::CANCELLED: return QColor(QStringLiteral("#4B5563"));
    case AppointmentState::RESCHEDULED: return QColor(QStringLiteral("#6D28D9"));
    case AppointmentState::NO_SHOW: return QColor(QStringLiteral("#991B1B"));
    }
    return {};
}

void TurnosModel::setItems(const QList<TurnoItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

const TurnoItem *TurnosModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return nullptr;
    }
    return &m_items.at(row);
}

QVariant TurnosModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) {
        return {};
    }

    const TurnoItem &item = m_items.at(index.row());
    const int column = index.column();

    switch (role) {
    case Qt::DisplayRole:
        switch (column) {
        case ColFecha:
            if (item.datetime.isValid()) {
                return item.datetime.toLocalTime().toString(QStringLiteral("dd/MM/yyyy HH:mm"));
            }
            return QStringLiteral("—");
        case ColPaciente:
            return QStringLiteral("%1 %2").arg(item.patientName, item.patientLastname);
        case ColTipo:
            return tipoLabel(item.type);
        case ColEstado:
            return appointmentStateLabel(item.state);
        case ColResponsable:
            return item.assignedUsers.isEmpty() ? QStringLiteral("Sin asignar") : item.assignedUsers;
        case ColObservacion:
            return item.observation.isEmpty() ? QStringLiteral("—") : item.observation;
        }
        return {};

    case Qt::BackgroundRole:
        if (column == ColEstado) {
            return tint(estadoColor(item.state), 0.12);
        }
        return {};

    case Qt::ForegroundRole:
        if (column == ColFecha || column == ColPaciente || column == ColObservacion
            || column == ColResponsable) {
            return QColor(QStringLiteral("#111827"));
        }
        if (column == ColEstado) {
            return estadoColor(item.state);
        }
        if (column == ColTipo) {
            return QColor(QStringLiteral("#374151"));
        }
        return {};

    case Qt::TextAlignmentRole:
        if (column == ColTipo || column == ColEstado) {
            return static_cast<int>(Qt::AlignCenter);
        }
        return {};

    case Qt::FontRole:
        if (column == ColPaciente) {
            QFont font;
            font.setWeight(QFont::DemiBold);
            return font;
        }
        return {};

    default:
        return {};
    }
}

QVariant TurnosModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case ColFecha: return QStringLiteral("Fecha y hora");
    case ColPaciente: return QStringLiteral("Paciente");
    case ColTipo: return QStringLiteral("Tipo");
    case ColEstado: return QStringLiteral("Estado");
    case ColResponsable: return QStringLiteral("Responsable asignado");
    case ColObservacion: return QStringLiteral("Observación");
    }
    return {};
}
