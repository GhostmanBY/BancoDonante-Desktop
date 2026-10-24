#ifndef UPDATERESTRAINTTEMPLATEDTO_H
#define UPDATERESTRAINTTEMPLATEDTO_H

#include <QJsonObject>
#include <QString>

class UpdateRestraintTemplateDTO
{
public:
    UpdateRestraintTemplateDTO(int id, QString motivo, int defaultMonths);

    virtual QJsonObject toJson() const;

    int id = 0;
    QString motivo;
    int defaultMonths = 1;
};

#endif
