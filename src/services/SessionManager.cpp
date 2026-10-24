#include "services/SessionManager.hpp"

SessionManager &SessionManager::instance()
{
    static SessionManager manager;
    return manager;
}

void SessionManager::setSession(const QByteArray &token, const SessionUser &user)
{
    m_token = token;
    m_user = user;
    m_authenticated = true;
    emit sessionChanged();
}

void SessionManager::clear()
{
    if (!m_authenticated && m_token.isEmpty()) {
        return;
    }

    m_token.clear();
    m_user = SessionUser{};
    m_authenticated = false;
    emit sessionChanged();
}

void SessionManager::expireSession()
{
    if (!m_authenticated) {
        return;
    }

    clear();
    emit sessionExpired();
}

bool SessionManager::isAuthenticated() const
{
    return m_authenticated;
}

QByteArray SessionManager::token() const
{
    return m_token;
}

SessionUser SessionManager::user() const
{
    return m_user;
}
