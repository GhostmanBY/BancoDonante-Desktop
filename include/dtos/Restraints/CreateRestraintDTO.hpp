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
        QDateTime estimatedEndDate,
        int templateId = 0);

    virtual QJsonObject toJson() const;

    QString motivo;
    QDateTime estimatedStartDate;
    QDateTime estimatedEndDate;
    // 0 = sin plantilla de catálogo; >0 deja constancia de la
    // procedencia sin modificar la plantilla.
    int templateId = 0;
};

#endif
