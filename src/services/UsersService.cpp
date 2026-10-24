#include "services/UsersService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace {

QJsonObject userBody(const UserItem &item, const QString &newPassword)
{
    QJsonObject body;
    body[QStringLiteral("name")] = item.name;
    body[QStringLiteral("lastname")] = item.lastname;
    body[QStringLiteral("mail")] = item.mail;
    body[QStringLiteral("phone")] = item.phone;
    body[QStringLiteral("document_type")] = item.documentType;
    body[QStringLiteral("document_number")] = item.documentNumber;
    body[QStringLiteral("birth_date")] = item.birthDate.isEmpty()
        ? QJsonValue(QJsonValue::Null)
        : QJsonValue(item.birthDate);
    body[QStringLiteral("birth_place")] = item.birthPlace.isEmpty()
        ? QJsonValue(QJsonValue::Null)
        : QJsonValue(item.birthPlace);
    body[QStringLiteral("username")] = item.username;
    body[QStringLiteral("hash_password")] = newPassword.isEmpty()
        ? QJsonValue(QJsonValue::Null)
        : QJsonValue(newPassword);
    body[QStringLiteral("role")] = item.role;
    return body;
}

} // namespace

UsersPage UsersPage::fromJson(const QJsonObject &json)
{
    UsersPage page;
    page.page = jsonutils::intOr(json, QStringLiteral("page"), 1);
    page.size = jsonutils::intOr(json, QStringLiteral("size"), 20);
    page.total = jsonutils::intOr(json, QStringLiteral("total"), 0);
    page.totalPages = jsonutils::intOr(json, QStringLiteral("total_pages"), 0);

    const QJsonArray items = json.value(QStringLiteral("data")).toArray();
    page.data.reserve(items.size());
    for (const QJsonValue &value : items) {
        const QJsonObject obj = value.toObject();

        UserItem item;
        item.id = jsonutils::intOr(obj, QStringLiteral("id"));
        item.username = jsonutils::stringOr(obj, QStringLiteral("username"));
        item.name = jsonutils::stringOr(obj, QStringLiteral("name"));
        item.lastname = jsonutils::stringOr(obj, QStringLiteral("lastname"));
        item.mail = jsonutils::stringOr(obj, QStringLiteral("mail"));
        item.phone = jsonutils::stringOr(obj, QStringLiteral("phone"));
        item.documentType = jsonutils::stringOr(obj, QStringLiteral("document_type"));
        item.documentNumber = jsonutils::stringOr(obj, QStringLiteral("document_number"));
        item.birthDate = jsonutils::stringOr(obj, QStringLiteral("birth_date"));
        item.birthPlace = jsonutils::stringOr(obj, QStringLiteral("birth_place"));
        item.isActive = jsonutils::boolOr(obj, QStringLiteral("isActive"), true);
        item.role = jsonutils::stringOr(obj, QStringLiteral("role"));

        page.data.append(item);
    }
    return page;
}

UsersService::UsersService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

void UsersService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void UsersService::handle(
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

void UsersService::fetchPage(int page, int size)
{
    const quint64 seq = ++m_pageSeq;

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("page"), QString::number(page));
    query.addQueryItem(QStringLiteral("size"), QString::number(size));

    prepare();
    QNetworkReply *reply = m_api.get(
        QStringLiteral("users?") + query.toString(QUrl::FullyEncoded));
    handle(reply, seq, [this, seq, page, size](int, const QByteArray &body) {
        if (seq != m_pageSeq) {
            return;
        }
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        UsersPage result = UsersPage::fromJson(json);
        result.page = page;
        result.size = size;
        emit pageFetched(result);
    });
}

void UsersService::create(const UserItem &item, const QString &password)
{
    prepare();
    QNetworkReply *reply = m_api.post(QStringLiteral("users"), userBody(item, password));
    handle(reply, 0, [this](int, const QByteArray &) {
        emit createSucceeded();
    });
}

void UsersService::update(int id, const UserItem &item, const QString &newPassword)
{
    prepare();
    QNetworkReply *reply = m_api.put(
        QStringLiteral("users/") + QString::number(id),
        userBody(item, newPassword));
    handle(reply, 0, [this](int, const QByteArray &) {
        emit updateSucceeded();
    });
}

void UsersService::setActive(int id, bool active)
{
    prepare();
    const QString path = active
        ? QStringLiteral("users/%1/activate").arg(id)
        : QStringLiteral("users/%1/deactivate").arg(id);
    QNetworkReply *reply = m_api.patch(path, QJsonObject());
    handle(reply, 0, [this](int, const QByteArray &) {
        emit activeChanged();
    });
}
