#include "services/TypeDonationService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

TypeDonationService::TypeDonationService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void TypeDonationService::fetchActive()
{
    m_api.setToken(SessionManager::instance().token());

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("size"), QStringLiteral("100"));

    QNetworkReply *reply = m_api.get(
        QStringLiteral("typedonations?") + query.toString(QUrl::FullyEncoded));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        const QVariant statusAttr =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const int status = statusAttr.isValid() ? statusAttr.toInt() : 0;
        const QByteArray body = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            emit failed(ApiError::fromJson(status, body));
            return;
        }

        const QJsonArray items =
            QJsonDocument::fromJson(body).object().value(QStringLiteral("data")).toArray();

        QList<TypeDonationItem> result;
        for (const QJsonValue &value : items) {
            const QJsonObject obj = value.toObject();
            if (!jsonutils::boolOr(obj, QStringLiteral("is_active"), true)
                && !jsonutils::boolOr(obj, QStringLiteral("isActive"), true)) {
                continue;
            }
            result.append({
                jsonutils::intOr(obj, QStringLiteral("id")),
                jsonutils::stringOr(obj, QStringLiteral("name")),
            });
        }

        emit fetched(result);
    });
}
