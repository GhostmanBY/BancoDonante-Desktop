#ifndef TOPBAR_H
#define TOPBAR_H

#include "services/SessionManager.hpp"
#include "services/WhatsAppService.hpp"

#include <QFrame>
#include <QHash>
#include <QList>
#include <QPointer>

class QDialog;
class QLabel;
class QPushButton;
class QTimer;

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
    QPushButton *m_reviewButton = nullptr;
    WhatsAppService *m_whatsAppService = nullptr;
    QTimer *m_reviewTimer = nullptr;
    QList<WSPInboundReview> m_pendingReviews;
    QHash<int, QPointer<QWidget>> m_reviewCards;
    QPointer<QDialog> m_reviewDialog;
};

#endif
