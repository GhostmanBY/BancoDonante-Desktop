#ifndef CYCLERULESERVICE_H
#define CYCLERULESERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QObject>
#include <QString>

struct CycleRuleItem
{
    int id = 0;
    int typeDonationId = 0;
    QString typeDonationName;
    QString sex;
    int maxDonations = 0;
    int waitDays = 0;
    int waitBetweenCyclesDays = 0;
    bool isActive = true;

    static CycleRuleItem fromJson(const QJsonObject &json);
};

class CycleRuleService : public QObject
{
    Q_OBJECT

public:
    explicit CycleRuleService(QObject *parent = nullptr);

    void fetchAll();
    void update(int id, int maxDonations, int waitDays, int waitBetweenCyclesDays, bool isActive);

signals:
    void fetched(const QList<CycleRuleItem> &rules);
    void updateSucceeded();
    void failed(const ApiError &error);

private:
    void prepare();
    void handle(QNetworkReply *reply, const std::function<void(int, const QByteArray &)> &onSuccess);

    ApiClient m_api;
};

#endif
