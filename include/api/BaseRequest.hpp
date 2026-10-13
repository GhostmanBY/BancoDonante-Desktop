
#ifndef BASEREQUEST_H
#define BASEREQUEST_H

#include <QNetworkRequest>
#include <QUrl>
#include <QByteArray>
#include <QString>

class BaseRequest
{
public:
    explicit BaseRequest(const QUrl &baseUrl);

    QNetworkRequest build(const QString &endpoint) const;

    void setToken(const QByteArray &token);

private:
    QUrl m_baseUrl;
    QByteArray m_token;
};

#endif
