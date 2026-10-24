#ifndef TYPEDONATIONSERVICE_H
#define TYPEDONATIONSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QObject>
#include <QString>

struct TypeDonationItem
{
    int id = 0;
    QString name;
};

class TypeDonationService : public QObject
{
    Q_OBJECT

public:
    explicit TypeDonationService(QObject *parent = nullptr);

    void fetchActive();

signals:
    void fetched(const QList<TypeDonationItem> &items);
    void failed(const ApiError &error);

private:
    ApiClient m_api;
};

#endif
