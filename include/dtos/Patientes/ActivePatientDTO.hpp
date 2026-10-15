#ifndef ACTIVEPATIENTDTO_H
#define ACTIVEPATIENTDTO_H

#include <QJsonObject>

class ActivePatientDTO
{
public:
    ActivePatientDTO(int id, bool isActive);

    QJsonObject toJson() const;

    int id;
    bool isActive;
};

#endif
