#include "services/RestraintService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

PatientObservationItem PatientObservationItem::fromJson(const QJsonObject &json)
{
    PatientObservationItem item;
    item.id = jsonutils::intOr(json, QStringLiteral("id"));
    item.patientId = jsonutils::intOr(json, QStringLiteral("patient_id"));
    item.texto = jsonutils::stringOr(json, QStringLiteral("texto"));
    item.createdAt = jsonutils::stringOr(json, QStringLiteral("created_at"));
    return item;
}

RestraintTemplateItem RestraintTemplateItem::fromJson(const QJsonObject &json)
{
    RestraintTemplateItem item;
    item.id = jsonutils::intOr(json, QStringLiteral("id"));
    item.motivo = jsonutils::stringOr(json, QStringLiteral("motivo"));
    item.defaultMonths = qMax(1, jsonutils::intOr(json, QStringLiteral("default_months"), 1));
    item.isActive = json.value(QStringLiteral("is_active")).toBool(true);
    return item;
}

RestraintService::RestraintService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void RestraintService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void RestraintService::handle(
    QNetworkReply *reply,
    const std::function<void(int, const QByteArray &)> &onSuccess)
{
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        reply->deleteLater();

        const QVariant statusAttr =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const int status = statusAttr.isValid() ? statusAttr.toInt() : 0;
        const QByteArray body = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            if (status == 401) {
                SessionManager::instance().expireSession();
            }
            emit failed(ApiError::fromJson(status, body));
            return;
        }

        onSuccess(status == 0 ? 200 : status, body);
    });
}

void RestraintService::assignRestraint(int patientId, const CreateRestraintDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.post(
        QStringLiteral("patientes/%1/restraints").arg(patientId), dto.toJson());
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        emit restraintAssigned(jsonutils::intOr(json, QStringLiteral("cancelled_appointments")));
    });
}

void RestraintService::fetchObservations(int patientId)
{
    prepare();
    QNetworkReply *reply = m_api.get(
        QStringLiteral("patientes/%1/observations").arg(patientId));
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        emit observationsFetched(json.value(QStringLiteral("items")).toArray());
    });
}

void RestraintService::fetchRestraints(int patientId)
{
    prepare();
    QNetworkReply *reply = m_api.get(
        QStringLiteral("patientes/%1/restraints").arg(patientId));
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonDocument document = QJsonDocument::fromJson(body);
        if (document.isArray()) {
            emit restraintsFetched(document.array());
            return;
        }

        const QJsonObject json = document.object();
        QJsonArray items = json.value(QStringLiteral("data")).toArray();
        if (items.isEmpty()) {
            items = json.value(QStringLiteral("items")).toArray();
        }
        if (items.isEmpty() && json.contains(QStringLiteral("motivo"))) {
            items.append(json);
        }
        emit restraintsFetched(items);
    });
}

void RestraintService::fetchTemplates()
{
    prepare();
    QNetworkReply *reply = m_api.get(QStringLiteral("restraint-templates"));
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        QList<RestraintTemplateItem> templates;
        const QJsonArray items = json.value(QStringLiteral("data")).toArray();
        templates.reserve(items.size());
        for (const QJsonValue &value : items) {
            templates.append(RestraintTemplateItem::fromJson(value.toObject()));
        }
        emit templatesFetched(templates);
    });
}

void RestraintService::createTemplate(const CreateRestraintTemplateDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.post(QStringLiteral("restraint-templates"), dto.toJson());
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        emit templateSaved(jsonutils::intOr(json, QStringLiteral("id")));
    });
}

void RestraintService::updateTemplate(int id, const UpdateRestraintTemplateDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.put(
        QStringLiteral("restraint-templates/%1").arg(id), dto.toJson());
    handle(reply, [this, id](int, const QByteArray &) {
        emit templateSaved(id);
    });
}
