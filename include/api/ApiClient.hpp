
#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QString>

#include "api/BaseRequest.hpp"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(
        const QUrl &baseUrl,
        QObject *parent = nullptr
    );

    QNetworkReply *get(const QString &endpoint);

    QNetworkReply *post(
        const QString &endpoint,
        const QJsonObject &body
    );

    QNetworkReply *put(
        const QString &endpoint,
        const QJsonObject &body
    );

    QNetworkReply *deleteResource(
        const QString &endpoint
    );

    QNetworkReply *patch(
        const QString &endpoint,
        const QJsonObject &body
    );

    void setToken(const QByteArray &token);

private:
    BaseRequest m_baseRequest;
    QNetworkAccessManager m_manager;
};

#endif
