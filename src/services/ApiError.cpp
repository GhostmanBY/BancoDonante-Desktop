#include "services/ApiError.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace {

QString defaultMessageFor(int status)
{
    switch (status) {
    case 0: return QStringLiteral("No se pudo conectar con el servidor");
    case 400: return QStringLiteral("Datos inválidos");
    case 401: return QStringLiteral("Sesión expirada o no autorizada");
    case 403: return QStringLiteral("No tenés permisos para realizar esta operación");
    case 404: return QStringLiteral("Recurso no encontrado");
    case 409: return QStringLiteral("Conflicto: el recurso no permite esta operación");
    default: return QStringLiteral("Error inesperado");
    }
}

} // namespace

ApiError ApiError::fromJson(int status, const QByteArray &body)
{
    ApiError error;
    error.status = status;

    const QJsonObject obj = QJsonDocument::fromJson(body).object();
    error.detail = obj.value(QLatin1String("detail")).toString();
    error.field = obj.value(QLatin1String("field")).toString();

    if (error.detail.isEmpty()) {
        error.detail = defaultMessageFor(status);
    }

    return error;
}

ApiError ApiError::fromReply(QNetworkReply *reply)
{
    const QVariant statusAttr =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusAttr.isValid()) {
        ApiError error;
        error.status = 0;
        error.detail = reply->errorString();
        if (error.detail.isEmpty()) {
            error.detail = defaultMessageFor(0);
        }
        return error;
    }

    return fromJson(statusAttr.toInt(), reply->readAll());
}
