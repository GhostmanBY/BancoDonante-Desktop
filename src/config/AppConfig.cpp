#include "config/AppConfig.hpp"

#include <QSettings>

namespace {
constexpr auto kBaseUrlKey = "api/baseUrl";
constexpr auto kRememberedUsernameKey = "auth/rememberedUsername";
constexpr auto kDefaultBaseUrl = "http://31.97.160.86/banDonante";
}

AppConfig &AppConfig::instance()
{
    static AppConfig config;
    return config;
}

QUrl AppConfig::baseUrl() const
{
    QSettings settings;
    const QString stored = settings.value(kBaseUrlKey).toString();
    if (stored.isEmpty()) {
        return QUrl(QLatin1String(kDefaultBaseUrl));
    }
    return QUrl(stored);
}

void AppConfig::setBaseUrl(const QUrl &url)
{
    QSettings settings;
    settings.setValue(kBaseUrlKey, url.toString());
}

QString AppConfig::rememberedUsername() const
{
    QSettings settings;
    return settings.value(kRememberedUsernameKey).toString();
}

void AppConfig::setRememberedUsername(const QString &username)
{
    QSettings settings;
    settings.setValue(kRememberedUsernameKey, username);
}
