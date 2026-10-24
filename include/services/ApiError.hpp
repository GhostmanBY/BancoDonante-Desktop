#ifndef APIERROR_H
#define APIERROR_H

#include <QByteArray>
#include <QString>

class QNetworkReply;

struct ApiError
{
    int status = 0;
    QString detail;
    QString field;

    bool isNetworkError() const { return status == 0; }

    static ApiError fromReply(QNetworkReply *reply);
    static ApiError fromJson(int status, const QByteArray &body);
};

#endif
