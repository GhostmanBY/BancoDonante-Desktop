#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QUrl>

class AppConfig
{
public:
    static AppConfig &instance();

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &url);

    QString rememberedUsername() const;
    void setRememberedUsername(const QString &username);
    
private:
    AppConfig() = default;
};

#endif
