#ifndef TURNOSMODEL_H
#define TURNOSMODEL_H

#include "services/AppointmentService.hpp"

#include <QAbstractTableModel>
#include <QColor>
#include <QList>

class TurnosModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        ColFecha = 0,
        ColPaciente,
        ColTipo,
        ColEstado,
        ColObservacion,
        ColumnCount,
    };

    explicit TurnosModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setItems(const QList<TurnoItem> &items);
    const TurnoItem *itemAt(int row) const;

    static QString tipoLabel(AppointmentType type);
    static QColor estadoColor(AppointmentState state);

private:
    static QColor tint(const QColor &color, double strength);

    QList<TurnoItem> m_items;
};

#endif
