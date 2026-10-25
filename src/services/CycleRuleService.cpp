#include "services/CycleRuleService.hpp"

#include "config/AppConfig.hpp"
#include "core/JsonUtils.hpp"
#include "services/SessionManager.hpp"

#include <QDate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

CycleRuleItem CycleRuleItem::fromJson(const QJsonObject &json)
{
    CycleRuleItem item;
    item.id = jsonutils::intOr(json, QStringLiteral("id"));
    item.typeDonationId = jsonutils::intOr(json, QStringLiteral("type_donation_id"));
    item.typeDonationName = jsonutils::stringOr(json, QStringLiteral("type_donation_name"));
    item.sex = jsonutils::stringOr(json, QStringLiteral("sex"));
    item.maxDonations = jsonutils::intOr(json, QStringLiteral("max_donations"));
    item.waitDays = jsonutils::intOr(json, QStringLiteral("wait_days"));
    item.waitBetweenCyclesDays = jsonutils::intOr(json, QStringLiteral("wait_between_cycles_days"));
    item.isActive = jsonutils::boolOr(json, QStringLiteral("is_active"), true);
    return item;
}

CycleRuleService::CycleRuleService(QObject *parent)
    : QObject(parent),
      m_api(AppConfig::instance().baseUrl(), this)
{
}

std::optional<CycleRuleItem> CycleRuleService::ruleFor(
    const QList<CycleRuleItem> &rules, const Patientes &donor)
{
    if (!donor.lastDonationTypeId().has_value()) {
        return std::nullopt;
    }

    const QString donorSex = ::toString(donor.sex());
    const CycleRuleItem *specific = nullptr;
    const CycleRuleItem *generic = nullptr;

    for (const CycleRuleItem &rule : rules) {
        if (!rule.isActive || rule.typeDonationId != *donor.lastDonationTypeId()) {
            continue;
        }
        if (rule.sex.isEmpty()) {
            if (!generic) {
                generic = &rule;
            }
        } else if (rule.sex == donorSex && !specific) {
            specific = &rule;
        }
    }

    if (specific) {
        return *specific;
    }
    if (generic) {
        return *generic;
    }
    return std::nullopt;
}

int CycleRuleService::waitDaysFor(const CycleRuleItem &rule, bool hasOpenCycle)
{
    return hasOpenCycle ? rule.waitDays : rule.waitBetweenCyclesDays;
}

std::optional<bool> CycleRuleService::restFulfilled(
    const QList<CycleRuleItem> &rules, const Patientes &donor)
{
    const auto rule = ruleFor(rules, donor);
    if (!rule.has_value()) {
        return std::nullopt;
    }
    if (!donor.lastDonationDate().has_value() || !donor.lastDonationDate()->isValid()) {
        return std::nullopt;
    }

    const int daysSince = -QDate::currentDate().daysTo(donor.lastDonationDate()->date());
    return daysSince >= rule->waitDays;
}

void CycleRuleService::prepare()
{
    m_api.setToken(SessionManager::instance().token());
}

void CycleRuleService::handle(
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

void CycleRuleService::fetchAll()
{
    prepare();
    QNetworkReply *reply = m_api.get(QStringLiteral("cyclerules"));
    handle(reply, [this](int, const QByteArray &body) {
        const QJsonObject json = QJsonDocument::fromJson(body).object();
        const QJsonArray items = json.value(QStringLiteral("data")).toArray();

        QList<CycleRuleItem> rules;
        rules.reserve(items.size());
        for (const QJsonValue &value : items) {
            rules.append(CycleRuleItem::fromJson(value.toObject()));
        }
        emit fetched(rules);
    });
}

void CycleRuleService::update(
    int id, int maxDonations, int waitDays, int waitBetweenCyclesDays, bool isActive)
{
    QJsonObject body;
    body.insert(QStringLiteral("max_donations"), maxDonations);
    body.insert(QStringLiteral("wait_days"), waitDays);
    body.insert(QStringLiteral("wait_between_cycles_days"), waitBetweenCyclesDays);
    body.insert(QStringLiteral("is_active"), isActive);

    prepare();
    QNetworkReply *reply =
        m_api.put(QStringLiteral("cyclerules/%1").arg(id), body);
    handle(reply, [this](int, const QByteArray &) {
        emit updateSucceeded();
    });
}
