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
            const QJsonObject response = QJsonDocument::fromJson(body).object();
            const QJsonArray data = response.value(QStringLiteral("data")).toArray();
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

void WhatsAppService::fetchTemplates()
{
    prepare();
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("size"), QStringLiteral("100"));
    handle(m_api.get(QStringLiteral("wsp-templates?") + query.toString(QUrl::FullyEncoded)),
        [this](const QByteArray &body) {
            const QJsonObject response = QJsonDocument::fromJson(body).object();
            const QJsonArray data = response.value(QStringLiteral("data")).toArray();
            QList<WSPTemplateItem> templates;
            templates.reserve(data.size());
            for (const QJsonValue &value : data) {
                const QJsonObject item = value.toObject();
                WSPTemplateItem templateItem;
                templateItem.id = item.value(QStringLiteral("id")).toInt();
                templateItem.motive = item.value(QStringLiteral("motive")).toString();
                templateItem.name = item.value(QStringLiteral("name")).toString();
                templateItem.text = item.value(QStringLiteral("text")).toString();
                templateItem.isActive = item.value(QStringLiteral("is_active")).toBool();
                if (templateItem.id > 0) {
                    templates.append(templateItem);
                }
            }
            emit managedTemplatesFetched(templates);
        });
}

void WhatsAppService::updateTemplate(int id, const QString &motive, const QString &name, const QString &text)
{
    prepare();
    QJsonObject body;
    body.insert(QStringLiteral("motive"), motive);
    body.insert(QStringLiteral("name"), name);
    body.insert(QStringLiteral("text"), text);
    handle(m_api.put(QStringLiteral("wsp-templates/%1").arg(id), body), [this, id](const QByteArray &) {
        emit templateSaved(id);
    });
}

void WhatsAppService::setTemplateActive(int id, bool active)
{
    prepare();
    const QString action = active ? QStringLiteral("activate") : QStringLiteral("deactivate");
    handle(m_api.patch(QStringLiteral("wsp-templates/%1/%2").arg(id).arg(action), {}),
        [this, id, active](const QByteArray &) {
            emit templateActivationChanged(id, active);
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

void WhatsAppService::fetchPendingInboundReviews()
{
    prepare();
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("size"), QStringLiteral("100"));
    handle(m_api.get(QStringLiteral("wsp-inbound-messages?") + query.toString(QUrl::FullyEncoded)),
        [this](const QByteArray &body) {
            const QJsonObject response = QJsonDocument::fromJson(body).object();
            const QJsonArray data = response.value(QStringLiteral("data")).toArray();
            const int total = response.value(QStringLiteral("total")).toInt(data.size());
            QList<WSPInboundReview> reviews;
            reviews.reserve(data.size());
            for (const QJsonValue &value : data) {
                const QJsonObject item = value.toObject();
                WSPInboundReview review;
                review.id = item.value(QStringLiteral("id")).toInt();
                review.patientId = item.value(QStringLiteral("patiente_id")).toInt();
                review.appointmentId = item.value(QStringLiteral("appointment_id")).toInt();
                review.patientName = item.value(QStringLiteral("patient_name")).toString();
                review.senderPhone = item.value(QStringLiteral("sender_phone")).toString();
                review.body = item.value(QStringLiteral("body")).toString();
                review.receivedAt = item.value(QStringLiteral("received_at")).toString();
                if (review.id > 0) {
                    reviews.append(review);
                }
            }
            emit inboundReviewsFetched(reviews, total);
        });
}

void WhatsAppService::markInboundReviewReviewed(int id)
{
    prepare();
    handle(m_api.patch(QStringLiteral("wsp-inbound-messages/%1/reviewed").arg(id), {}),
        [this, id](const QByteArray &) {
            emit inboundReviewMarked(id);
        });
}

void WhatsAppService::resolveInboundReview(int id, const QString &decision)
{
    prepare();
    QJsonObject body;
    body.insert(QStringLiteral("decision"), decision);
    handle(m_api.patch(QStringLiteral("wsp-inbound-messages/%1/resolve").arg(id), body),
        [this, id](const QByteArray &) {
            emit inboundReviewResolved(id);
        });
}
