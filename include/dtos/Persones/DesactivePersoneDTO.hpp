#ifndef DESACTIVEPERSONEDTO_H
#define DESACTIVEPERSONEDTO_H

#include <QJsonObject>

class DesactivePersoneDTO
{
public:
    DesactivePersoneDTO(int id, bool isActive);

    QJsonObject toJson() const;

    int id;
    bool isActive;
};

#endif
