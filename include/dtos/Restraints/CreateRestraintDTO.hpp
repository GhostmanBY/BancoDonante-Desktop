#ifndef CREATERESTRAINTDTO_H
#define CREATERESTRAINTDTO_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>

class CreateRestraintDTO
{
public:
    CreateRestraintDTO(
        QString motivo,
        QDateTime estimatedStartDate,
        QDateTime estimatedEndDate);

    virtual QJsonObject toJson() const;

    QString motivo;
    QDateTime estimatedStartDate;
    QDateTime estimatedEndDate;
};

#endif
