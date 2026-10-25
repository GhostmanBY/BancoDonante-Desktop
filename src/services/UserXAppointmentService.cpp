#include "services/UserXAppointmentService.hpp"

#include "config/AppConfig.hpp"
#include "services/SessionManager.hpp"

#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

UserXAppointmentService::UserXAppointmentService(QObject *parent)
    : QObject(parent), m_api(AppConfig::instance().baseUrl(), this)
{
}

void UserXAppointmentService::create(int userId, int appointmentId)
{
    m_api.setToken(SessionManager::instance().token());
    QJsonObject body;
    body.insert(QStringLiteral("user_id"), userId);
    body.insert(QStringLiteral("appointment_id"), appointmentId);
    QNetworkReply *reply = m_api.post(QStringLiteral("usersxappointments"), body);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        const QVariant statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const int status = statusAttr.isValid() ? statusAttr.toInt() : 0;
        const QByteArray response = reply->readAll();
        if (reply->error() != QNetworkReply::NoError) {
            if (status == 401) SessionManager::instance().expireSession();
            emit failed(ApiError::fromJson(status, response));
            return;
        }
        emit createSucceeded();
    });
}
