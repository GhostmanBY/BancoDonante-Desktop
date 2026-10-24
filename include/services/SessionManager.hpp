#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "models/enums/UserRole.hpp"

#include <QByteArray>
#include <QObject>
#include <QString>

struct SessionUser
{
    int id = 0;
    QString name;
    QString lastname;
    UserRole role = UserRole::NURSE;

    QString fullName() const
    {
        return QStringLiteral("%1 %2").arg(name, lastname);
    }
};

class SessionManager : public QObject
{
    Q_OBJECT

public:
    static SessionManager &instance();

    void setSession(const QByteArray &token, const SessionUser &user);
    void clear();
    void expireSession();

    bool isAuthenticated() const;
    QByteArray token() const;
    SessionUser user() const;

signals:
    void sessionChanged();
    void sessionExpired();

private:
    SessionManager() = default;

    QByteArray m_token;
    SessionUser m_user;
    bool m_authenticated = false;
};

#endif
