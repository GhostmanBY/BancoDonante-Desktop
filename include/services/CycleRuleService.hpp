#ifndef CYCLERULESERVICE_H
#define CYCLERULESERVICE_H

#include "api/ApiClient.hpp"
#include "models/Patientes.hpp"
#include "services/ApiError.hpp"

#include <QList>
#include <QObject>
#include <QString>
#include <optional>

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

    // Resuelve la regla aplicable a un donante: la del tipo de su ultima
    // donacion, activa y compatibile con su sexo. Si hay una regla especifica
    // para el sexo gana sobre la generica (mismo criterio que el backend usa al
    // registrar la donacion).
    static std::optional<CycleRuleItem> ruleFor(
        const QList<CycleRuleItem> &rules,
        const Patientes &donor);

    // Espera que aplica al completar el turno: entre donaciones si hay serie
    // abierta, entre series si hay que iniciar una nueva.
    static int waitDaysFor(const CycleRuleItem &rule, bool hasOpenCycle);

    // Espera minima (wait_days) respecto de hoy. nullopt = no hay regla o el
    // donante nunca dono, y ahi no se pinta nada.
    static std::optional<bool> restFulfilled(
        const QList<CycleRuleItem> &rules,
        const Patientes &donor);

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
