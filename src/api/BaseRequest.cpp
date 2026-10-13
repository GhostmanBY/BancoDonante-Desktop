#include "api/BaseRequest.hpp"

BaseRequest::BaseRequest(const QUrl &baseurl) : m_baseUrl(baseurl) {}

void BaseRequest::setToken(const QByteArray &token) {
    m_token = token;
}

QNetworkRequest BaseRequest::build(const QString &endpoint) const {
    QUrl url = m_baseUrl;
    QString path = url.path();

    if (!path.endsWith('/'))
        path += '/';

    path += endpoint;
    url.setPath(path);

    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
    );

    request.setRawHeader(
        "Accept",
        "application/json"
    );

    if (!m_token.isEmpty()){
        request.setRawHeader(
            "Authorization",
            "Bearer" + m_token
        );
    }

    return request;
}

