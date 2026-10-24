#include "services/DonorService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace {

QByteArray pageQuery(int page, int size)
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QString::number(page));
    query.addQueryItem(QStringLiteral("size"), QString::number(size));
    return query.toString(QUrl::FullyEncoded).toUtf8();
}

QByteArray searchQuery(const QString &search)
{
    if (search.isEmpty()) {
        return {};
    }
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("search"), search);
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

DonantesPage DonantesPage::fromJson(const QJsonObject &json)
{
    DonantesPage page;
    page.page = jsonutils::intOr(json, QStringLiteral("page"), 1);
    page.size = jsonutils::intOr(json, QStringLiteral("size"), 20);
    page.total = jsonutils::intOr(json, QStringLiteral("total"), 0);
    page.totalPages = jsonutils::intOr(json, QStringLiteral("total_pages"), 0);

    const QJsonArray items = json.value(QStringLiteral("data")).toArray();
    page.data.reserve(items.size());
    for (const QJsonValue &value : items) {
        page.data.append(Patientes::fromJson(value.toObject()));
    }
    return page;
}

DonorService::DonorService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void DonorService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void DonorService::handle(
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

void DonorService::fetchPage(
    int page,
    int size,
    const QString &search,
    const std::optional<PatientState> &state,
    const std::optional<bool> &isActive)
{
    const quint64 seq = ++m_pageSeq;

    QByteArray query = pageQuery(page, size);

    const QByteArray searchPart = searchQuery(search);
    if (!searchPart.isEmpty()) {
        query += '&' + searchPart;
    }

    if (state.has_value()) {
        QUrlQuery extra;
        extra.addQueryItem(QStringLiteral("state"), ::toString(*state));
        query += '&' + extra.toString(QUrl::FullyEncoded).toUtf8();
    }

    if (isActive.has_value()) {
        QUrlQuery extra;
        extra.addQueryItem(QStringLiteral("is_active"), *isActive ? QStringLiteral("true") : QStringLiteral("false"));
        query += '&' + extra.toString(QUrl::FullyEncoded).toUtf8();
    }

    prepare();
    QNetworkReply *reply = m_api.get(withQuery(QStringLiteral("patientes"), query));
    handle(reply, seq, [this, seq, page, size](int, const QByteArray &body) {
        if (seq != m_pageSeq) {
            return;
        }
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        DonantesPage result = DonantesPage::fromJson(json);
        result.page = page;
        result.size = size;
        emit pageFetched(result);
    });
}

void DonorService::create(const CreatePatientesDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.post(QStringLiteral("patientes"), dto.toJson());
    handle(reply, 0, [this](int, const QByteArray &body) {
        emit createSucceeded(
            Patientes::fromJson(QJsonDocument::fromJson(body).object()));
    });
}

void DonorService::update(const UpdatePatientesDTO &dto)
{
    prepare();
    QNetworkReply *reply =
        m_api.put(QStringLiteral("patientes/%1").arg(dto.id), dto.toJson());
    handle(reply, 0, [this](int, const QByteArray &) {
        emit updateSucceeded();
    });
}

void DonorService::activate(int id)
{
    prepare();
    QNetworkReply *reply =
        m_api.patch(QStringLiteral("patientes/%1/activate").arg(id), QJsonObject{});
    handle(reply, 0, [this](int, const QByteArray &) {
        emit toggleSucceeded();
    });
}

void DonorService::deactivate(int id)
{
    prepare();
    QNetworkReply *reply =
        m_api.patch(QStringLiteral("patientes/%1/deactivate").arg(id), QJsonObject{});
    handle(reply, 0, [this](int, const QByteArray &) {
        emit toggleSucceeded();
    });
}

void DonorService::fetchAddresses(int personId, int page, int size)
{
    prepare();
    QNetworkReply *reply = m_api.get(withQuery(
        QStringLiteral("persones/%1/addresses").arg(personId),
        pageQuery(page, size)));
    handle(reply, 0, [this](int, const QByteArray &body) {
        emit addressesFetched(QJsonDocument::fromJson(body).object());
    });
}

void DonorService::createAddress(const CreateAddressDTO &dto)
{
    prepare();
    QNetworkReply *reply = m_api.post(QStringLiteral("addresses"), dto.toJson());
    handle(reply, 0, [this](int, const QByteArray &) {
        emit addressCreated();
    });
}

void DonorService::makeCurrentAddress(int addressId)
{
    prepare();
    QNetworkReply *reply = m_api.patch(
        QStringLiteral("addresses/%1/make-current").arg(addressId), QJsonObject{});
    handle(reply, 0, [this](int, const QByteArray &) {
        emit addressCurrentSet();
    });
}

void DonorService::deleteAddress(int addressId)
{
    prepare();
    QNetworkReply *reply =
        m_api.deleteResource(QStringLiteral("addresses/%1").arg(addressId));
    handle(reply, 0, [this](int, const QByteArray &) {
        emit addressDeleted();
    });
}

void DonorService::fetchCycles(int patientId, int page, int size)
{
    prepare();
    QNetworkReply *reply = m_api.get(withQuery(
        QStringLiteral("patientes/%1/cycles").arg(patientId),
        pageQuery(page, size)));
    handle(reply, 0, [this](int, const QByteArray &body) {
        emit cyclesFetched(QJsonDocument::fromJson(body).object());
    });
}

void DonorService::fetchCycleDetail(int cycleId)
{
    prepare();
    QNetworkReply *reply = m_api.get(
        QStringLiteral("cycles/%1").arg(cycleId));
    handle(reply, 0, [this](int, const QByteArray &body) {
        emit cycleDetailFetched(QJsonDocument::fromJson(body).object());
    });
}
