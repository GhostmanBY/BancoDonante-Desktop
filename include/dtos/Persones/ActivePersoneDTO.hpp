#ifndef ACTIVEPERSONEDTO_H
#define ACTIVEPERSONEDTO_H

#include <QJsonObject>

class ActivePersoneDTO
{
public:
    ActivePersoneDTO(int id, bool isActive);

    QJsonObject toJson() const;

    int id;
    bool isActive;
};

#endif
