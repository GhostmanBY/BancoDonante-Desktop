#include "services/DashboardService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

DashboardStats DashboardStats::fromJson(const QJsonObject &json)
{
    DashboardStats stats;
    stats.donantesActivos = jsonutils::intOr(json, QStringLiteral("donantes_activos"));
    stats.donantesSinContacto = jsonutils::intOr(json, QStringLiteral("donantes_sin_contacto"));
    stats.donacionesMes = jsonutils::intOr(json, QStringLiteral("donaciones_mes"));

    const QJsonObject byState =
        json.value(QStringLiteral("donantes_por_estado")).toObject();
    stats.donantesApto = jsonutils::intOr(byState, QStringLiteral("apto"));
    stats.donantesRevision = jsonutils::intOr(byState, QStringLiteral("revision"));
    stats.donantesNoApto = jsonutils::intOr(byState, QStringLiteral("no-apto"));
    stats.donantesSuspended = jsonutils::intOr(byState, QStringLiteral("suspended"));

    const QJsonObject today = json.value(QStringLiteral("turnos_hoy")).toObject();
    stats.turnosHoyPendientes = jsonutils::intOr(today, QStringLiteral("pending"));
    stats.turnosHoyConfirmados = jsonutils::intOr(today, QStringLiteral("confirmed"));

    return stats;
}

DashboardService::DashboardService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void DashboardService::fetchStats()
{
    m_api.setToken(SessionManager::instance().token());

    QNetworkReply *reply = m_api.get(QStringLiteral("dashboard/stats"));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        const QVariant statusAttr =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const int status = statusAttr.isValid() ? statusAttr.toInt() : 0;
        const QByteArray body = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            if (status == 401) {
                SessionManager::instance().expireSession();
            }
            emit failed(ApiError::fromJson(status, body));
            return;
        }

        emit statsFetched(DashboardStats::fromJson(
            QJsonDocument::fromJson(body).object()));
    });
}
