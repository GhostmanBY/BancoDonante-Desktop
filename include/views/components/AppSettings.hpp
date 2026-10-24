#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

class AppSettings
{
public:
    static AppSettings &instance()
    {
        static AppSettings settings;
        return settings;
    }

    int restDays() const
    {
        return m_settings.value(QStringLiteral("restDays"), 14).toInt();
    }

    void setRestDays(int days)
    {
        m_settings.setValue(QStringLiteral("restDays"), days);
    }

private:
    AppSettings() = default;

    QSettings m_settings;
};

#endif
