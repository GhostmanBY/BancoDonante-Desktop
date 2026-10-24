#ifndef DONANTESMODEL_H
#define DONANTESMODEL_H

#include "models/Patientes.hpp"

#include <QAbstractTableModel>
#include <QColor>
#include <QList>

class DonantesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        ColDonante = 0,
        ColDocumento,
        ColGrupo,
        ColEstado,
        ColUltimaDonacion,
        ColMail,
        ColTelefono,
        ColActivo,
        ColumnCount,
    };

    explicit DonantesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setItems(const QList<Patientes> &items);
    const Patientes *itemAt(int row) const;

    static QColor donationGradient(const std::optional<QDateTime> &lastDonation);

private:
    static QString estadoLabel(PatientState state);
    static QColor estadoColor(PatientState state);
    static QColor tint(const QColor &color, double strength);

    QList<Patientes> m_items;
};

#endif
