#include "services/AppointmentService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include <memory>

namespace {

QByteArray pageQuery(int page, int size)
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QString::number(page));
    query.addQueryItem(QStringLiteral("size"), QString::number(size));
    return query.toString(QUrl::FullyEncoded).toUtf8();
}

QString withQuery(const QString &path, const QByteArray &query)
{
    if (query.isEmpty()) {
        return path;
    }
    return path + QLatin1Char('?') + QString::fromUtf8(query);
}

} // namespace

TurnosPage TurnosPage::fromJson(const QJsonObject &json)
{
    TurnosPage page;
    page.page = jsonutils::intOr(json, QStringLiteral("page"), 1);
    page.size = jsonutils::intOr(json, QStringLiteral("size"), 20);
    page.total = jsonutils::intOr(json, QStringLiteral("total"), 0);
    page.totalPages = jsonutils::intOr(json, QStringLiteral("total_pages"), 0);

    const QJsonArray items = json.value(QStringLiteral("data")).toArray();
    page.data.reserve(items.size());
    for (const QJsonValue &value : items) {
        const QJsonObject obj = value.toObject();

        TurnoItem item;
        item.id = jsonutils::intOr(obj, QStringLiteral("id"));
        item.datetime = jsonutils::parseDateTime(
            jsonutils::stringOr(obj, QStringLiteral("appointment_datetime")));
        item.state = appointmentStateFromString(
            jsonutils::stringOr(obj, QStringLiteral("state")),
            AppointmentState::PENDING);
        item.type = appointmentTypeFromString(
            jsonutils::stringOr(obj, QStringLiteral("type")),
            AppointmentType::DONATION);
        item.patientId = jsonutils::intOr(obj, QStringLiteral("patient_id"));
        item.patientName = jsonutils::stringOr(obj, QStringLiteral("patient_name"));
        item.patientLastname = jsonutils::stringOr(obj, QStringLiteral("patient_lastname"));
        item.assignedUsers = jsonutils::stringOr(obj, QStringLiteral("assigned_users"));
        item.observation = jsonutils::stringOr(obj, QStringLiteral("observation"));
        if (obj.contains(QStringLiteral("type_donation_id"))
            && !obj.value(QStringLiteral("type_donation_id")).isNull()) {
            item.typeDonationId = jsonutils::intOr(obj, QStringLiteral("type_donation_id"));
        }

        page.data.append(item);
    }
    return page;
}

AppointmentService::AppointmentService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void AppointmentService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void AppointmentService::handle(
    QNetworkReply *reply,
    quint64 requestSeq,
    const std::function<void(int, const QByteArray &)> &onSuccess)
{
    connect(reply, &QNetworkReply::finished, this, [this, reply, requestSeq, onSuccess]() {
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
    Q_UNUSED(requestSeq);
}

void AppointmentService::fetchPage(
    int page,
    int size,
    const std::optional<AppointmentState> &state)
{
    const quint64 seq = ++m_pageSeq;

    QByteArray query = pageQuery(page, size);

    if (state.has_value()) {
        QUrlQuery extra;
        extra.addQueryItem(QStringLiteral("state"), ::toString(*state));
        query += '&' + extra.toString(QUrl::FullyEncoded).toUtf8();
    }

    prepare();
    QNetworkReply *reply = m_api.get(withQuery(QStringLiteral("appointments"), query));
    handle(reply, seq, [this, seq, page, size](int, const QByteArray &body) {
        if (seq != m_pageSeq) {
            return;
        }
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        TurnosPage result = TurnosPage::fromJson(json);
        result.page = page;
        result.size = size;
        emit pageFetched(result);
    });
}

void AppointmentService::create(const CreateAppointmentDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.post(QStringLiteral("appointments"), dto.toJson());
    handle(reply, 0, [this](int, const QByteArray &) {
        emit createSucceeded();
    });
}

void AppointmentService::update(const UpdateAppointmentDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.put(
        QStringLiteral("appointments/%1").arg(dto.id), dto.toJson());
    handle(reply, 0, [this](int, const QByteArray &) {
        emit updateSucceeded();
    });
}

void AppointmentService::changeState(int id, AppointmentState state)
{
    prepare();

    QJsonObject body;
    body.insert(QStringLiteral("state"), ::toString(state));

    QNetworkReply *reply = m_api.patch(
        QStringLiteral("appointments/%1/state").arg(id), body);
    handle(reply, 0, [this](int, const QByteArray &) {
        emit changeStateSucceeded();
    });
}

void AppointmentService::autoNoShowExpired(const QList<int> &ids)
{
    if (ids.isEmpty()) {
        emit expiredNoShowDone();
        return;
    }

    auto remaining = std::make_shared<int>(ids.size());

    prepare();
    for (int id : ids) {
        QJsonObject body;
        body.insert(QStringLiteral("state"), ::toString(AppointmentState::NO_SHOW));

        QNetworkReply *reply = m_api.patch(
            QStringLiteral("appointments/%1/state").arg(id), body);
        connect(reply, &QNetworkReply::finished, this, [this, reply, remaining]() {
            reply->deleteLater();

            const QVariant statusAttr =
                reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            const int status = statusAttr.isValid() ? statusAttr.toInt() : 0;
            if (status == 401) {
                SessionManager::instance().expireSession();
            }

            // Sincronización en segundo plano: los errores se ignoran.
            if (--(*remaining) <= 0) {
                emit expiredNoShowDone();
            }
        });
    }
}
