
#include "api/ApiClient.hpp"

#include <QJsonDocument>
#include <QNetworkRequest>

ApiClient::ApiClient(
    const QUrl &baseUrl,
    QObject *parent
)
    : QObject(parent),
      m_baseRequest(baseUrl),
      m_manager(this)
{
}

QNetworkReply *ApiClient::get(const QString &endpoint)
{
    QNetworkRequest request = m_baseRequest.build(endpoint);

    return m_manager.get(request);
}

QNetworkReply *ApiClient::post(
    const QString &endpoint,
    const QJsonObject &body
)
{
    QNetworkRequest request =
        m_baseRequest.build(endpoint);

    QByteArray json =
        QJsonDocument(body).toJson(QJsonDocument::Compact);

    return m_manager.post(request, json);
}

QNetworkReply *ApiClient::put(
    const QString &endpoint,
    const QJsonObject &body
) 
{
    QNetworkRequest request = m_baseRequest.build(endpoint);

    QByteArray json = QJsonDocument(body).toJson(QJsonDocument::Compact);

    return m_manager.put(request, json);
}

QNetworkReply *ApiClient::patch(
    const QString &endpoint,
    const QJsonObject &body
) {
    QNetworkRequest request = m_baseRequest.build(endpoint);

    QByteArray json = QJsonDocument(body).toJson(QJsonDocument::Compact);

    return m_manager.sendCustomRequest(
        request,
        "PATCH",
        json
    );
}

QNetworkReply *ApiClient::deleteResource(const QString &endpoint) {
    QNetworkRequest request = m_baseRequest.build(endpoint);
    
    return m_manager.deleteResource(request);
}

void ApiClient::setToken(const QByteArray &token)
{
    m_baseRequest.setToken(token);
}
