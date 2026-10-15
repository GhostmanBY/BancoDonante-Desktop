#ifndef DEACTIVEPATIENTDTO_H
#define DEACTIVEPATIENTDTO_H

#include <QJsonObject>

class DeactivePatientDTO
{
public:
    DeactivePatientDTO(int id, bool isActive);

    QJsonObject toJson() const;

    int id;
    bool isActive;
};

#endif
