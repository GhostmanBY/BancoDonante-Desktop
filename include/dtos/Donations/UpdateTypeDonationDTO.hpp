#ifndef UPDATETYPEDONATIONDTO_H
#define UPDATETYPEDONATIONDTO_H

#include <QJsonObject>
#include <QString>

class UpdateTypeDonationDTO
{
public:
    UpdateTypeDonationDTO(int id, QString name);

    QJsonObject toJson() const;

    int id;
    QString name;
};

#endif
