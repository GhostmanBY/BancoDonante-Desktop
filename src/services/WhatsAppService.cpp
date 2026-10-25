#include "services/WhatsAppService.hpp"

#include "config/AppConfig.hpp"
#include "services/SessionManager.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonArray>

WhatsAppService::WhatsAppService(QObject *parent)
    : QObject(parent), m_api(AppConfig::instance().baseUrl(), this)
{
}

void WhatsAppService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void WhatsAppService::handle(
    QNetworkReply *reply,
    const std::function<void(const QByteArray &)> &onSuccess)
{
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QByteArray body = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            if (status == 401) {
                SessionManager::instance().expireSession();
            }
            emit failed(ApiError::fromJson(status, body));
            return;
        }

        onSuccess(body);
    });
}

void WhatsAppService::fetchConnection()
{
    prepare();
    handle(m_api.get(QStringLiteral("whatsapp/connection")), [this](const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        emit connectionFetched(
            json.value(QStringLiteral("state")).toString(QStringLiteral("offline")),
            json.value(QStringLiteral("qr")).toString());
    });
}

void WhatsAppService::fetchActiveTemplates()
{
    prepare();
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("size"), QStringLiteral("100"));
    handle(m_api.get(QStringLiteral("wsp-templates?") + query.toString(QUrl::FullyEncoded)),
        [this](const QByteArray &body) {
            const QJsonArray data = QJsonDocument::fromJson(body).object()
                .value(QStringLiteral("data")).toArray();
            QList<WSPTemplateOption> templates;
            for (const QJsonValue &value : data) {
                const QJsonObject item = value.toObject();
                if (!item.value(QStringLiteral("is_active")).toBool()) {
                    continue;
                }
                const QString motive = item.value(QStringLiteral("motive")).toString();
                if (!motive.isEmpty()) {
                    templates.append({motive, item.value(QStringLiteral("name")).toString(motive)});
                }
            }
            emit templatesFetched(templates);
        });
}

void WhatsAppService::enqueue(int patientId, const QString &motive)
{
    prepare();
    QJsonObject body;
    body.insert(QStringLiteral("patiente_id"), patientId);
    body.insert(QStringLiteral("motive"), motive);
    handle(m_api.post(QStringLiteral("wsp-queues"), body), [this](const QByteArray &) {
        emit enqueueSucceeded();
    });
}
