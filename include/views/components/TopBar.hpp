#ifndef TOPBAR_H
#define TOPBAR_H

#include "services/SessionManager.hpp"

#include <QFrame>

class QLabel;
class QPushButton;
class WhatsAppService;

class TopBar : public QFrame
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = nullptr);

    void setUser(const SessionUser &user);

signals:
    void logoutRequested();

private:
    QLabel *m_nameLabel = nullptr;
    QLabel *m_roleLabel = nullptr;
    QPushButton *m_logoutButton = nullptr;
    QPushButton *m_qrButton = nullptr;
    WhatsAppService *m_whatsAppService = nullptr;
};

#endif
