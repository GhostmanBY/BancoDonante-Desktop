#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "api/ApiClient.hpp"
#include "services/ApiError.hpp"
#include "services/SessionManager.hpp"

#include <QObject>
#include <QString>

class AuthService : public QObject
{
    Q_OBJECT

public:
    explicit AuthService(QObject *parent = nullptr);

    void login(const QString &username, const QString &password);
    bool isBusy() const;

signals:
    void loginSucceeded(const SessionUser &user);
    void loginFailed(const ApiError &error);

private:
    ApiClient m_api;
    bool m_busy = false;
};

#endif
