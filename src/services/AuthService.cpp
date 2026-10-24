#include "services/AuthService.hpp"

#include "config/AppConfig.hpp"
#include "models/enums/UserRole.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

AuthService::AuthService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

bool AuthService::isBusy() const
{
    return m_busy;
}

void AuthService::login(const QString &username, const QString &password)
{
    if (m_busy) {
        return;
    }

    m_busy = true;

    QJsonObject body;
    body[QLatin1String("username")] = username;
    body[QLatin1String("password")] = password;

    QNetworkReply *reply = m_api.post(QStringLiteral("auth/login"), body);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_busy = false;

        const QVariant statusAttr =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

        if (reply->error() != QNetworkReply::NoError) {
            emit loginFailed(ApiError::fromReply(reply));
            return;
        }

        const int status = statusAttr.isValid() ? statusAttr.toInt() : 200;
        const QByteArray raw = reply->readAll();

        const QJsonObject obj = QJsonDocument::fromJson(raw).object();
        const QString token = obj.value(QLatin1String("token")).toString();
        const QJsonObject userObj = obj.value(QLatin1String("user")).toObject();

        if (token.isEmpty() || userObj.isEmpty()) {
            emit loginFailed(ApiError::fromJson(
                status == 200 ? 500 : status,
                raw
            ));
            return;
        }

        SessionUser user;
        user.id = userObj.value(QLatin1String("id")).toInt();
        user.name = userObj.value(QLatin1String("name")).toString();
        user.lastname = userObj.value(QLatin1String("lastname")).toString();
        user.role = userRoleFromString(
            userObj.value(QLatin1String("role")).toString(),
            UserRole::NURSE
        );

        SessionManager::instance().setSession(token.toUtf8(), user);
        emit loginSucceeded(user);
    });
}
