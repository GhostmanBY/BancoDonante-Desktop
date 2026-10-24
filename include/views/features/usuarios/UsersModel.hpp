#ifndef USERSMODEL_H
#define USERSMODEL_H

#include "services/UsersService.hpp"

#include <QAbstractTableModel>
#include <QColor>
#include <QList>

class UsersModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        ColUsuario = 0,
        ColNombre,
        ColMail,
        ColRol,
        ColActivo,
        ColumnCount,
    };

    explicit UsersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setItems(const QList<UserItem> &items);
    const UserItem *itemAt(int row) const;

private:
    static QColor tint(const QColor &color, double strength);
    static QColor rolColor(const QString &role);

    QList<UserItem> m_items;
};

#endif
