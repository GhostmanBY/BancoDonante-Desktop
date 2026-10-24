#include "api/BaseRequest.hpp"

BaseRequest::BaseRequest(const QUrl &baseurl) : m_baseUrl(baseurl) {}

void BaseRequest::setToken(const QByteArray &token) {
    m_token = token;
}

QNetworkRequest BaseRequest::build(const QString &endpoint) const {
    QUrl url = m_baseUrl;

    const int queryPos = endpoint.indexOf(QLatin1Char('?'));
    const QString pathPart =
        queryPos >= 0 ? endpoint.left(queryPos) : endpoint;
    const QString queryPart =
        queryPos >= 0 ? endpoint.mid(queryPos + 1) : QString();

    QString path = url.path();
    if (!path.endsWith('/'))
        path += '/';

    path += pathPart;
    url.setPath(path);

    if (!queryPart.isEmpty())
        url.setQuery(queryPart);

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
            "Bearer " + m_token
        );
    }

    return request;
}

