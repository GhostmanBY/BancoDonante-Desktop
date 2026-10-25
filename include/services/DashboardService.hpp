#ifndef DASHBOARDSERVICE_H
#define DASHBOARDSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"

#include <QJsonObject>
#include <QObject>

struct DashboardStats
{
    int donantesActivos = 0;
    int donantesApto = 0;
    int donantesRevision = 0;
    int donantesNoApto = 0;
    int donantesSuspended = 0;
    int donantesSinContacto = 0;
    int turnosHoyPendientes = 0;
    int turnosHoyConfirmados = 0;
    int turnosHoyRealizados = 0;
    int donacionesMes = 0;

    static DashboardStats fromJson(const QJsonObject &json);
};

class DashboardService : public QObject
{
    Q_OBJECT

public:
    explicit DashboardService(QObject *parent = nullptr);

    void fetchStats();

signals:
    void statsFetched(const DashboardStats &stats);
    void failed(const ApiError &error);

private:
    ApiClient m_api;
};

#endif
