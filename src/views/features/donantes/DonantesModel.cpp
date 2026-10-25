#include "views/features/donantes/DonantesModel.hpp"

#include <QDate>
#include <QFont>

namespace {

QColor blendOnWhite(const QColor &color, double strength)
{
    const double r = 255.0 + (color.red() - 255.0) * strength;
    const double g = 255.0 + (color.green() - 255.0) * strength;
    const double b = 255.0 + (color.blue() - 255.0) * strength;
    return QColor(qRound(r), qRound(g), qRound(b));
}

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

QColor estadoColor(PatientState state)
{
    switch (state) {
    case PatientState::APTO: return QColor(QStringLiteral("#065F46"));
    case PatientState::REVISON: return QColor(QStringLiteral("#92400E"));
    case PatientState::NO_APTO: return QColor(QStringLiteral("#991B1B"));
    case PatientState::SUSPENDED: return QColor(QStringLiteral("#4B5563"));
    }
    return {};
}

} // namespace

DonantesModel::DonantesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int DonantesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

int DonantesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QColor DonantesModel::donationGradient(const std::optional<QDateTime> &lastDonation)
{
    const QColor red(QStringLiteral("#DC2626"));
    const QColor green(QStringLiteral("#16A34A"));

    if (!lastDonation.has_value() || !lastDonation->isValid()) {
        return green;
    }

    const int days = QDate::currentDate().daysTo(lastDonation->date());
    const double t = qBound(0.0, static_cast<double>(days) / 365.0, 1.0);

    const double r = red.red() + (green.red() - red.red()) * t;
    const double g = red.green() + (green.green() - red.green()) * t;
    const double b = red.blue() + (green.blue() - red.blue()) * t;

    return QColor(qRound(r), qRound(g), qRound(b));
}

QColor DonantesModel::tint(const QColor &color, double strength)
{
    return blendOnWhite(color, strength);
}

QString DonantesModel::estadoLabel(PatientState state)
{
    return ::estadoLabel(state);
}

QColor DonantesModel::estadoColor(PatientState state)
{
    return ::estadoColor(state);
}

void DonantesModel::setItems(const QList<Patientes> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

void DonantesModel::setCycleRules(const QList<CycleRuleItem> &rules)
{
    m_rules = rules;
    if (m_items.isEmpty()) {
        return;
    }
    emit dataChanged(index(0, 0), index(m_items.size() - 1, ColumnCount - 1));
}

const Patientes *DonantesModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return nullptr;
    }
    return &m_items.at(row);
}

QVariant DonantesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size()) {
        return {};
    }

    const Patientes &item = m_items.at(index.row());
    const int column = index.column();

    switch (role) {
    case Qt::DisplayRole:
        switch (column) {
        case ColDonante:
            return QStringLiteral("%1 %2").arg(item.name(), item.lastname());
        case ColDocumento:
            return QStringLiteral("%1 %2").arg(item.documentType(), item.documentNumber());
        case ColGrupo:
            return item.bloodGroup().has_value()
                ? ::toString(*item.bloodGroup())
                : QStringLiteral("—");
        case ColEstado:
            return estadoLabel(item.stateDonation());
        case ColUltimaDonacion:
            if (item.lastDonationDate().has_value() && item.lastDonationDate()->isValid()) {
                const int days = qAbs(QDate::currentDate().daysTo(item.lastDonationDate()->date()));
                return QStringLiteral("%1 (hace %2 d)")
                    .arg(item.lastDonationDate()->date().toString(QStringLiteral("dd/MM/yyyy")))
                    .arg(days);
            }
            return QStringLiteral("Sin donaciones");
        case ColMail:
            return item.mail();
        case ColTelefono:
            return item.phone();
        case ColActivo:
            return item.isActive() ? QStringLiteral("Sí") : QStringLiteral("No");
        }
        return {};

    case Qt::BackgroundRole:
        if (column == ColEstado) {
            return tint(estadoColor(item.stateDonation()), 0.12);
        }
        if (column == ColUltimaDonacion) {
            if (item.lastDonationDate().has_value() && item.lastDonationDate()->isValid()) {
                const auto cumplido = CycleRuleService::restFulfilled(m_rules, item);
                if (!cumplido.has_value()) {
                    return tint(QColor(QStringLiteral("#6B7280")), 0.10);
                }
                const QColor strong = *cumplido
                    ? QColor(QStringLiteral("#15803D"))
                    : QColor(QStringLiteral("#DC2626"));
                return tint(strong, 0.14);
            }
            return tint(donationGradient(item.lastDonationDate()), 0.14);
        }
        if (column == ColActivo && !item.isActive()) {
            return tint(QColor(QStringLiteral("#6B7280")), 0.08);
        }
        return {};

    case Qt::ForegroundRole:
        if (column == ColDonante || column == ColDocumento
            || column == ColMail || column == ColTelefono) {
            return QColor(QStringLiteral("#111827"));
        }
        if (column == ColEstado) {
            return estadoColor(item.stateDonation());
        }
        if (column == ColUltimaDonacion) {
            if (item.lastDonationDate().has_value() && item.lastDonationDate()->isValid()) {
                const auto cumplido = CycleRuleService::restFulfilled(m_rules, item);
                if (!cumplido.has_value()) {
                    return QColor(QStringLiteral("#6B7280"));
                }
                return *cumplido
                    ? QColor(QStringLiteral("#15803D"))
                    : QColor(QStringLiteral("#DC2626"));
            }
            const QColor strong = donationGradient(item.lastDonationDate());
            return QColor(qMax(0, strong.red() - 70), qMax(0, strong.green() - 40), qMax(0, strong.blue() - 70));
        }
        if (column == ColActivo) {
            return item.isActive()
                ? QColor(QStringLiteral("#065F46"))
                : QColor(QStringLiteral("#6B7280"));
        }
        if (column == ColGrupo) {
            return QColor(QStringLiteral("#B91C1C"));
        }
        return {};

    case Qt::TextAlignmentRole:
        if (column == ColGrupo || column == ColEstado || column == ColActivo) {
            return static_cast<int>(Qt::AlignCenter);
        }
        return {};

    case Qt::FontRole:
        if (column == ColDonante) {
            QFont font;
            font.setWeight(QFont::DemiBold);
            return font;
        }
        if (column == ColGrupo) {
            QFont font;
            font.setWeight(QFont::Bold);
            return font;
        }
        return {};

    default:
        return {};
    }
}

QVariant DonantesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case ColDonante: return QStringLiteral("Donante");
    case ColDocumento: return QStringLiteral("Documento");
    case ColGrupo: return QStringLiteral("Grupo");
    case ColEstado: return QStringLiteral("Estado");
    case ColUltimaDonacion: return QStringLiteral("Última donación");
    case ColMail: return QStringLiteral("Mail");
    case ColTelefono: return QStringLiteral("Teléfono");
    case ColActivo: return QStringLiteral("Activo");
    }
    return {};
}
