#ifndef CREATERESTRAINTTEMPLATEDTO_H
#define CREATERESTRAINTTEMPLATEDTO_H

#include <QJsonObject>
#include <QString>

class CreateRestraintTemplateDTO
{
public:
    CreateRestraintTemplateDTO(QString motivo, int defaultMonths);

    virtual QJsonObject toJson() const;

    QString motivo;
    int defaultMonths = 1;
};

#endif
